//
// Unit tests for alert system
//

#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>

#include "alert.h"
#include "base58.h"
#include "key.h"
#include "serialize.h"
#include "util.h"

CAlert Sign(const CUnsignedAlert &alert, const CKey &key)
{
    CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
    sMsg << alert;

    CAlert signedAlert;
    signedAlert.vchMsg.clear();
    signedAlert.vchMsg.resize(sMsg.size());
    std::copy(sMsg.begin(), sMsg.end(), signedAlert.vchMsg.begin());

    auto hash = Hash(signedAlert.vchMsg.begin(), signedAlert.vchMsg.end());
    key.Sign(hash, signedAlert.vchSig);
    return signedAlert;
}

//
// alertTests contains 7 alerts, generated with this code:
// (SignAndSave code not shown, alert signing key is secret)
//
void GenerateTestFile(boost::filesystem::path testFile, const char* base58SecretKey)
{
    CBitcoinSecret secret;
    secret.SetString(base58SecretKey);
    CKey key = secret.GetKey();

    FILE* fp = fopen(testFile.string().c_str(), "wb");
    if (!fp) return;

    CAutoFile fileout = CAutoFile(fp, SER_DISK, CLIENT_VERSION);
    if (!fileout) return;

    CUnsignedAlert alert;
    alert.nRelayUntil   = 60;
    alert.nExpiration   = 24 * 60 * 60;
    alert.nID           = 1;
    alert.nCancel       = 0;   // cancels previous messages up to this ID number
    alert.nMinVer       = 0;  // These versions are protocol versions
    alert.nMaxVer       = 70001;
    alert.nPriority     = 1;
    alert.strComment    = "Alert comment";
    alert.strStatusBar  = "Alert 1";

    fileout << Sign(alert, key);

    alert.setSubVer.insert(std::string("/Satoshi:0.1.0/"));
    alert.strStatusBar  = "Alert 1 for Satoshi 0.1.0";
    fileout << Sign(alert, key);

    alert.setSubVer.insert(std::string("/Satoshi:0.2.0/"));
    alert.strStatusBar  = "Alert 1 for Satoshi 0.1.0, 0.2.0";
    fileout << Sign(alert, key);

    alert.setSubVer.clear();
    ++alert.nID;
    alert.nCancel = 1;
    alert.nPriority = 100;
    alert.strStatusBar  = "Alert 2, cancels 1";
    fileout << Sign(alert, key);

    alert.nExpiration += 60;
    ++alert.nID;
    fileout << Sign(alert, key);

    ++alert.nID;
    alert.nMinVer = 11;
    alert.nMaxVer = 22;
    fileout << Sign(alert, key);

    ++alert.nID;
    alert.strStatusBar  = "Alert 2 for Satoshi 0.1.0";
    alert.setSubVer.insert(std::string("/Satoshi:0.1.0/"));
    fileout << Sign(alert, key);

    ++alert.nID;
    alert.nMinVer = 0;
    alert.nMaxVer = 999999;
    alert.strStatusBar  = "Evil Alert'; /bin/ls; echo '";
    alert.setSubVer.clear();
    fileout << Sign(alert, key);
}

struct ReadAlerts
{
    ReadAlerts()
    {
        std::string filename("alertTests");
        namespace fs = boost::filesystem;
        fs::path testFile = fs::current_path() / "test" / "data" / filename;
#ifdef TEST_DATA_DIR
        if (!fs::exists(testFile))
        {
            testFile = fs::path(BOOST_PP_STRINGIZE(TEST_DATA_DIR)) / filename;
        }
#endif
//        GenerateTestFile(testFile, "HERE GOES BASE58 PRIVATE ALERT KEY");

        FILE* fp = fopen(testFile.string().c_str(), "rb");
        if (!fp) return;


        CAutoFile filein = CAutoFile(fp, SER_DISK, CLIENT_VERSION);
        if (!filein) return;

        try {
            while (!feof(filein))
            {
                CAlert alert;
                filein >> alert;
                alerts.push_back(alert);
            }
        }
        catch (std::exception) { }
    }
    ~ReadAlerts() { }

    static std::vector<std::string> read_lines(boost::filesystem::path filepath)
    {
        std::vector<std::string> result;

        std::ifstream f(filepath.string().c_str());
        std::string line;
        while (std::getline(f,line))
            result.push_back(line);

        return result;
    }

    std::vector<CAlert> alerts;
};

BOOST_FIXTURE_TEST_SUITE(Alert_tests, ReadAlerts)


BOOST_AUTO_TEST_CASE(AlertApplies)
{
    SetMockTime(11);

    BOOST_FOREACH(const CAlert& alert, alerts)
    {
        BOOST_CHECK(alert.CheckSignature());
    }
    // Matches:
    BOOST_CHECK(alerts[0].AppliesTo(1, ""));
    BOOST_CHECK(alerts[0].AppliesTo(70001, ""));
    BOOST_CHECK(alerts[0].AppliesTo(1, "/Satoshi:11.11.11/"));

    BOOST_CHECK(alerts[1].AppliesTo(1, "/Satoshi:0.1.0/"));
    BOOST_CHECK(alerts[1].AppliesTo(70001, "/Satoshi:0.1.0/"));

    BOOST_CHECK(alerts[2].AppliesTo(1, "/Satoshi:0.1.0/"));
    BOOST_CHECK(alerts[2].AppliesTo(1, "/Satoshi:0.2.0/"));

    // Don't match:
    BOOST_CHECK(!alerts[0].AppliesTo(-1, ""));
    BOOST_CHECK(!alerts[0].AppliesTo(70002, ""));

    BOOST_CHECK(!alerts[1].AppliesTo(1, ""));
    BOOST_CHECK(!alerts[1].AppliesTo(1, "Satoshi:0.1.0"));
    BOOST_CHECK(!alerts[1].AppliesTo(1, "/Satoshi:0.1.0"));
    BOOST_CHECK(!alerts[1].AppliesTo(1, "Satoshi:0.1.0/"));
    BOOST_CHECK(!alerts[1].AppliesTo(-1, "/Satoshi:0.1.0/"));
    BOOST_CHECK(!alerts[1].AppliesTo(70002, "/Satoshi:0.1.0/"));
    BOOST_CHECK(!alerts[1].AppliesTo(1, "/Satoshi:0.2.0/"));

    BOOST_CHECK(!alerts[2].AppliesTo(1, "/Satoshi:0.3.0/"));

    SetMockTime(0);
}


// This uses sh 'echo' to test the -alertnotify function, writing to a
// /tmp file. So skip it on Windows:
#ifndef WIN32
BOOST_AUTO_TEST_CASE(AlertNotify)
{
    SetMockTime(11);

    boost::filesystem::path temp = GetTempPath() / "alertnotify.txt";
    boost::filesystem::remove(temp);

    mapArgs["-alertnotify"] = std::string("echo %s >> ") + temp.string();

    BOOST_FOREACH(CAlert alert, alerts)
        alert.ProcessAlert(false);

    std::vector<std::string> r = read_lines(temp);
    BOOST_CHECK_EQUAL(r.size(), 1u);
    BOOST_CHECK_EQUAL(r[0], "Evil Alert; /bin/ls; echo "); // single-quotes should be removed

    boost::filesystem::remove(temp);

    SetMockTime(0);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
