#include "CJsonObject.hpp"
#include "visa.h"

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "let_api.h"

#define query_( qproc ) if ( qproc != ERR_NONE ){ return ERR_FAIL_QUERY; }
#define deload_item( getter )   if ( getter ){}else{ return ERR_FAIL_GET; }
#define checked_call( proc )  ret = proc;if( ret != ERR_NONE ){ return ret; }
#define wait_idle( )    return letWaitIdle( vi, tmo );
int doSendCmd( int vi, neb::CJsonObject &obj )
{
    std::string fmtStr = obj.ToString();
    fmtStr += "#";

    ViUInt32 retLen;
    ViStatus viSta;
    viSta = viWrite( vi, (ViPBuf)fmtStr.data(), fmtStr.length(), &retLen );
    if ( viSta != VI_SUCCESS || retLen != fmtStr.length() )
    { return ERR_FAIL_SEND; }

    return ERR_NONE;
}

int sendCmd( int vi, neb::CJsonObject &obj )
{
    ViStatus viSta;
    viSta = viLock( vi, VI_EXCLUSIVE_LOCK, 0, VI_NULL, VI_NULL );
    if ( viSta != VI_SUCCESS
         && viSta != VI_SUCCESS_NESTED_EXCLUSIVE )
    { return ERR_FAIL_LOCK; }

        int ret;
        ret = doSendCmd( vi, obj );

    viUnlock( vi );

    return ret;
}

int queryCmd( int vi,
              neb::CJsonObject &obj,
              neb::CJsonObject &outObj )
{
    ViStatus viSta;
    viSta = viLock( vi, VI_EXCLUSIVE_LOCK, 0, VI_NULL, VI_NULL );
    if ( viSta != VI_SUCCESS
         && viSta != VI_SUCCESS_NESTED_EXCLUSIVE )
    { return ERR_FAIL_LOCK; }

    int ret = ERR_NONE;
    ViByte buf[512];
    do
    {
        ret = doSendCmd( vi,obj );
        if ( ret != ERR_NONE )
        { break; }


        ViUInt32 retCnt;
        ViStatus sta;
        sta = viRead( vi, buf, sizeof(buf)-1, &retCnt );
        if ( sta != VI_SUCCESS
             && sta != VI_SUCCESS_MAX_CNT
             && sta != VI_SUCCESS_TERM_CHAR )
        {
            ret = ERR_FAIL_READ;
            break;
        }
        else
        //! cat the \0
        { buf[retCnt] = '\0'; }
    }while( 0 );

    viUnlock( vi );

    if ( ret != ERR_NONE )
    { return ret; }

    std::string str( (char*)buf );
    if ( outObj.Parse( str ) )
    { }
    else
    { return ERR_FAIL_PARSE; }

    return ERR_NONE;
}

int letOpen( const std::string &ip )
{
    std::string rsrc = "TCPIP::" + ip + "::2345::SOCKET";

    ViStatus viSta;
    ViSession viDef, viDev;

    viSta = viOpenDefaultRM( &viDef );
    if ( viSta != VI_SUCCESS )
    { return ERR_FAIL_OPEN; }

    viSta = viOpen( viDef, rsrc.data(), 0, 0, &viDev );
    if ( viSta != VI_SUCCESS )
    { return ERR_FAIL_OPEN; }

    viSta = viSetAttribute( viDev, VI_ATTR_TERMCHAR_EN, VI_TRUE );
    if ( viSta != VI_SUCCESS )
    { return ERR_FAIL_SET_ATTR; }

    return viDev;
}

int letClose( int vi )
{
    viClose( vi );
    return ERR_NONE;
}

int _letWaitX( int vi,
               const std::string &str,
               const std::string &str2,
               const std::string &str3,
               std::string &outStr,
               int tmoms )
{
    int ret;

    //! no wait
    if ( tmoms == 0 )
    { return WARN_NO_WAIT; }

    std::string status;
    do
    {
        ret = letQueryStatus( vi, status );
        if ( ret != ERR_NONE )
        { return ret; }

        outStr = status;
        if ( status == str )
        { return ERR_NONE; }
        else if ( status == str2 )
        { return ERR_NONE; }
        else if ( status == str3 )
        { return ERR_NONE; }
        else
        {}
#ifdef _MSC_VER
        Sleep( 100 );       //! ms
#else
        usleep( 100000 );   //! 100ms
#endif

        tmoms -= 100;
    }while( tmoms > 0 );

    return ERR_FAIL_TMO;
}

int letWaitIdle( int vi, int tmoms )
{
    std::string outStr;
    return _letWaitX( vi, "stoped", "", "", outStr, tmoms );
}

int letWaitPending( int vi, std::string &status, int tmoms )
{
    return _letWaitX( vi, "pending", "stoped", "", status, tmoms );
}

///! request
int _letRequest( int vi,
                const std::string &item)
{
    neb::CJsonObject obj;

    obj.Add( "command", "request" );
    obj.Add( "item", item );

    return sendCmd( vi, obj );
}

int _letRequest( int vi,
                const std::string &item,
                float v )
{
    neb::CJsonObject obj;

    obj.Add( "command", "request" );
    obj.Add( "item", item );
    obj.Add( "velocity", v );

    return sendCmd( vi, obj );
}

int _letRequest( int vi,
                 const std::string &item,
                 float x, float y, float z,
                 float v,
                 int n = -1 )
{
    neb::CJsonObject obj;

    obj.Add( "command", "request" );
    obj.Add( "item", item );

    obj.Add( "x", x );
    obj.Add( "y", y );
    obj.Add( "z", z );
    obj.Add( "velocity", v );
    if ( n > 0 )
    {
        obj.Add( "n", n );
    }

    return sendCmd( vi, obj );
}

int letHome( int vi, float v, int tmo )
{
    int ret;

    checked_call( _letRequest( vi, "home", v ) );

    wait_idle();
}

int letHomeO( int vi, float v, int tmo )
{
    int ret;

    checked_call( _letRequest( vi, "homeo", v ) );

    wait_idle();
}

int letHomeZ( int vi, float v, int tmo )
{
    int ret;

    checked_call( _letRequest( vi, "homez", v ) );

    wait_idle();
}

int letTo( int vi, float x, float y, float v, int tmo )
{
    int ret;

    checked_call( _letRequest( vi, "to", x,y,0,v ) );

    wait_idle();
}


int letOrigin( int vi, float x, float y, float z, float v, int tmo )
{
    int ret;

    checked_call( _letRequest( vi, "origin", x,y,z,v ) );

    wait_idle();
}

int letZigzagx( int vi, float x, float y, float v )
{
    return _letRequest( vi, "zigzagx", x,y,0,v );
}

int letZigzagy( int vi, float x, float y, float v )
{
    return _letRequest( vi, "zigzagy", x,y, 0 ,v );
}

int letSnakex( int vi, float x, float y, float v )
{
    return _letRequest( vi, "snakex", x,y,0,v );
}

int letSnakey( int vi, float x, float y, float v )
{
    return _letRequest( vi, "snakey", x,y,0, v );
}

int letSlope( int vi, float x, float y, float v )
{
    return _letRequest( vi, "slope", x,y,0,v );
}

int letStep( int vi, float x, float y, float v, int n )
{
    return _letRequest( vi, "step", x, y, 0, v, n );
}

int letStepZ( int vi, float dz, float v, int tmo )
{
    int ret;

    checked_call( _letRequest( vi, "step", 0, 0, dz, v, 1 ) );

    wait_idle();
}

int letContinue( int vi )
{
    return _letRequest( vi, "continue" );
}

int letStop( int vi )
{
    return _letRequest( vi, "stop" );
}

int letEStop( int vi )
{
    return _letRequest( vi, "estop" );
}

///! config
int _config( const std::string &item, neb::CJsonObject &obj )
{
    obj.Add( "command", "config" );
    obj.Add( "item", item );

    return ERR_NONE;
}

int letConfigRst( int vi )
{
    neb::CJsonObject obj;

    _config( "rst", obj );

    return sendCmd( vi, obj );
}

int letConfigOrigin( int vi, float x, float y, float z )
{
    neb::CJsonObject obj;

    _config( "origin", obj );

    obj.Add( "x", x );
    obj.Add( "y", y );
    obj.Add( "z", z );

    return sendCmd( vi, obj );
}

int letConfigDir( int vi, int dirx, int diry )
{
    neb::CJsonObject obj;

    _config( "dir", obj );

    obj.Add( "dirx", dirx );
    obj.Add( "diry", diry );

    return sendCmd( vi, obj );
}

int letConfigWh( int vi, float w, float h )
{
    neb::CJsonObject obj;

    _config( "whz", obj );

    obj.Add( "w", w );
    obj.Add( "h", h );
    obj.Add( "d", 0 );

    return sendCmd( vi, obj );
}

int letConfigDwDh( int vi, float dw, float dh )
{
    neb::CJsonObject obj;

    _config( "dwdhdz", obj );

    obj.Add( "dw", dw );
    obj.Add( "dh", dh );
    obj.Add( "dd", 0 );

    return sendCmd( vi, obj );
}

int letConfigRv( int vi, float rv )
{
    neb::CJsonObject obj;

    _config( "rv", obj );

    obj.Add( "rv", rv );

    return sendCmd( vi, obj );
}

int letConfigZGap( int vi, float gap, float gapspeed, float zhomespeed )
{
    neb::CJsonObject obj;

    _config( "gap", obj );

    obj.Add( "gap", gap );
    obj.Add( "gapspeed", gapspeed );
    obj.Add( "zhomespeed", zhomespeed );

    return sendCmd( vi, obj );
}

int _query( const std::string &str, neb::CJsonObject &obj )
{
    obj.Add( "command", "query" );
    obj.Add( "item", str );

    return ERR_NONE;
}

int letQueryConfigOrigin( int vi, float &x, float &y, float &z )
{
    neb::CJsonObject obj;

    _query( "config", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "x", x ) );
    deload_item( qObj.Get( "y", y ) );
    deload_item( qObj.Get( "z", z ) );

    return ERR_NONE;
}

int letQueryConfigWhz( int vi, float &w, float &h, float &d )
{
    neb::CJsonObject obj;

    _query( "config", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "w", w ) );
    deload_item( qObj.Get( "h", h ) );
    deload_item( qObj.Get( "d", d ) );

    return ERR_NONE;
}

int letQueryConfigDir( int vi, int &dirx, int &diry )
{
    neb::CJsonObject obj;

    _query( "config", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "dirx", dirx ) );
    deload_item( qObj.Get( "diry", diry ) );

    return ERR_NONE;
}

int letQueryConfigDwDhDd( int vi, float &dw, float &dh, float &dd )
{
    neb::CJsonObject obj;

    _query( "config", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "dw", dw ) );
    deload_item( qObj.Get( "dh", dh ) );
    deload_item( qObj.Get( "dd", dd ) );

    return ERR_NONE;
}

int letQueryConfigRv( int vi, float &rv )
{
    neb::CJsonObject obj;

    _query( "config", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "rv", rv ) );

    return ERR_NONE;
}

int letQueryConfigGap( int vi, float &gap, float &gapspeed, float &zhomespeed )
{
    neb::CJsonObject obj;

    _query( "config", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "gap", gap ) );
    deload_item( qObj.Get( "gapspeed", gapspeed ) );
    deload_item( qObj.Get( "zhomespeed", zhomespeed ) );

    return ERR_NONE;
}

int letQueryPose( int vi, float &x, float &y, float &z )
{
    neb::CJsonObject obj;

    _query( "pose", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "x", x ) );
    deload_item( qObj.Get( "y", y ) );
    deload_item( qObj.Get( "z", z ) );

    return ERR_NONE;
}

int letQueryStatus( int vi, std::string &stat )
{
    neb::CJsonObject obj;

    _query( "status", obj );

    neb::CJsonObject qObj;
    query_( queryCmd( vi, obj, qObj ) );

    deload_item( qObj.Get( "status", stat ) );

    return ERR_NONE;
}





