#include <string>
#include <iostream>
#include "../let_api.h"

using namespace std;

int testConfig(int vi)
{
    //! config
    //    {
    letConfigRst(vi);

    letConfigOrigin(vi, 0, 0, 0);

    letConfigDir(vi, 1, 1);

    letConfigWh(vi, 100, 200);

    //        letConfigDwDh( vi, 11, 21 );

    //        letConfigRv( vi, 50 );

    letConfigZGap(vi, 6, 13, 15);
    //    }


    //! get info
    {
        float x, y, z;
        letQueryConfigOrigin(vi, x, y, z);
        cout << "queryConfigOrigin" << x << y << z << endl;

        letQueryConfigWhz(vi, x, y, z);
        cout << "queryConfigWhz" << x << y << z << endl;

        int dirx, diry;
        letQueryConfigDir(vi, dirx, diry);
        cout << "queryConfigDir" << dirx << diry << endl;

        letQueryConfigDwDhDd(vi, x, y, z);
        cout << "queryConfigDwDhDd" << x << y << z << endl;

        float rv;
        letQueryConfigRv(vi, rv);
        cout << "queryConfigRv" << rv << endl;
    }

    //! get gap
    {
        float gap, gapspeed, zhomespeed;
        letQueryConfigGap(vi, gap, gapspeed, zhomespeed);
        cout << "queryConfigGap" << gap << gapspeed << zhomespeed << endl;

        float x, y, z;
        letQueryPose(vi, x, y, z);
        cout << "queryPose" << x << y << z << endl;

        string status;
        letQueryStatus(vi, status);
        cout << "queryStatus" << status << endl;
    }

    return 0;
}

int testHome(int vi)
{   
    int ret;

    //ret = letHomeO(vi, 50);
    ret = letHome(vi, 100);
    //ret = letTo(vi, 100, 50, 50);
    //letStepZ( vi, 100, 1 );
    
    return 0;
}

int testScan(int vi)
{
    int ret;

    string outStr;
    //    ret = letSnakex( vi, 1, 1, 50 );
    //    ret = letSnakey( vi, 1, 1, 50 );
    //    ret = letZigzagx( vi,1,1,50);
    //    ret = letZigzagy( vi,1,1,50);
    //    ret = letSlope( vi, 100,100, 50 );
    ret = letStep(vi, 10, 10, 10, 5);
    do
    {
        ret = letWaitPending(vi, outStr);
        if (ret != ERR_NONE)
        {
            break;
        }

        if (outStr == "pending")
        {
        }
        else
        {
            break;
        }

        ret = letContinue(vi);
        if (ret != ERR_NONE)
        {
            break;
        }

    } while (1);

    return ret; 
}

int main()
{
    int ret;
    int vi;

    vi =letOpen();
    if ( vi < 0 )
    {
        cout<<"fail"<<endl;
        return -1;
    }

    testHome(vi);

    //testScan( vi );

    letClose( vi );

    return 0;
}
