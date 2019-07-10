#ifndef LET_API_H
#define LET_API_H

#include <string>

/** \file
 * \mainpage LET移动平台API描述
 *
 * # API有3类
 * * 设置： 设置工作空间，初始位置等
 * * 查询： 查询设置的参数，位置等
 * * 操作： 运动，扫描
 * # 设备操作流程
 * * 启动设备
 * * 连接设备
 * * 进行行回零操作：\note ***注意回零xy和z是分开的，z有掉电抱闸，z调整好后不需要每次都回零***,
 * * 进行移动或扫描等
 * # 运动中的等待
 * * 调用运动函数时设备默认为等待一段时间
 * * 如果在规定的时间里，设备没有运动结束，则会返回超时错误
 * # 返回错误
 * * 没有特殊说明时：返回值的含义为 letError
 * # 其他
 * * 设备默认地址169.254.1.2
 * * 程序在msvc编译器下和mingw下可以正常编译
 * * 注意msvc编译器下的4996警告
 * * 依赖于visa.h和visa.lib
 * # 源码说明
 * * cJson.c CJsonObject.cpp：JSON格式转换
 * * let_api.cpp: 操作api
 * * demo/let_test.cpp: 测试例程
*/


/**
 * @brief 错误码标记
 */
enum letError
{
 ERR_NONE = 0,              //! 无错误
 ERR_FAIL_SEND = -255,      //! 发送错误
 ERR_FAIL_READ,             //! 读取错误
 ERR_FAIL_PARSE,            //! 读取内容解释错误

 ERR_FAIL_OPEN,             //! 打开失败
 ERR_FAIL_SET_ATTR,         //! 设置属性失败
 ERR_FAIL_QUERY,            //! 查询失败

 ERR_FAIL_GET,              //! 从内容中提取数据失败

 ERR_FAIL_LOCK,             //! 锁定设备失败

 ERR_FAIL_TMO,              //! 设备操作超时

 WARN_NO_WAIT,              //! 警告，没有等待
};

#define def_tmo     60000   //! 单位，毫秒

/**
 * @brief 打开设备
 * @param ip
 * @return 设备句柄
 * @retval <0 打开失败
 * @retval 其他 成功
 */
int letOpen( const std::string &ip="169.254.1.2" );
//!
//! \brief 关闭设备
//! \param vi
//! \return
//!
int letClose( int vi );

//!
//! \brief 等待运行结束
//! \param vi
//! \param tmo
//! \return
//!
int letWaitIdle( int vi, int tmo=def_tmo );
//!
//! \brief 分布运行中的等待运行结束
//! * 扫描分步进行，每进行一步都会进入到"等待"状态
//! * 等到用户continue后开始下一步运动
//! * 整个扫描结束后，stat返回"stoped"
//! \param vi
//! \param stat
//! \param tmo
//! \see letContinue
//! \return
//!
int letWaitPending( int vi, std::string &stat, int tmo=def_tmo );

//!
//! \brief 回工作零位
//! 工作零位是用户设置的工作起始位
//! \param vi
//! \param v
//! \param tmo
//! \return
//!
int letHome( int vi, float v=10, int tmo=def_tmo );
//!
//! \brief 回设备零位(右上角)
//! 设备零位固定在右上角
//! \param vi
//! \param v
//! \param tmo
//! \return
//!
int letHomeO( int vi, float v=10, int tmo=def_tmo );
//!
//! \brief Z轴回零位
//! * Z轴的回零方向是向上
//! * 检测到限位开关后详细运动一个间隙距离
//! \see letConfigZGap
//! \param vi
//! \param v
//! \param tmo
//! \return
//!
int letHomeZ( int vi, float v=10, int tmo=def_tmo );

//!
//! \brief 到平面某个位置
//! * x,y依据的是工作起点
//! \param vi
//! \param x
//! \param y
//! \param v
//! \param tmo
//! \return
//!
int letTo( int vi, float x, float y, float v, int tmo=def_tmo );
//!
//! \brief X轴Z型扫描
//! * 先X轴运动到工作范围边界
//! * Y移动一个步进
//! * X反向移动到另一个边界
//! \param vi
//! \param x 扫描时的X方向
//! * 1 : 扫描时X+
//! * -1: 扫描时X-
//! \param y 扫描时的Y方向
//! * 1 : 扫描时Y+
//! * -1: 扫描时Y-
//! \param v
//! \return
//!
int letZigzagx( int vi, float x, float y, float v );
//!
//! \brief Y轴Z型扫描
//! * 先Y轴运动到工作范围边界
//! * X移动一个步进
//! * Y反向移动到另一个边界
//! \see letZigzagx
//! \param vi
//! \param x 扫描时的方向
//! * 1
//! * -1
//! \param y 扫描时的方向
//! * 1
//! * -1
//! \param v
//! \return
//!
int letZigzagy( int vi, float x, float y, float v );

//!
//! \brief X轴蛇形扫描
//! \param vi
//! \param x 扫描时的方向
//! * 1
//! * -1
//! \param y 扫描时的方向
//! * 1
//! * -1
//! \param v
//! \see letZigzagx
//! \return
//!
int letSnakex( int vi, float x, float y, float v );
//!
//! \brief Y轴蛇形扫描
//! \param vi
//! \param x 扫描时的方向
//! * 1
//! * -1
//! \param y 扫描时的方向
//! * 1
//! * -1
//! \param v
//! \see letZigzagx
//! \return
//!
int letSnakey( int vi, float x, float y, float v );
//!
//! \brief 斜线扫描
//! * 在当前位置和(x,y)进行斜线扫描
//! * 步进分别为dw,dh
//! * 步数以小的为准
//! \param vi
//! \param x 目标x
//! \param y 目标y
//! \param v
//! \return
//!
int letSlope( int vi, float x, float y, float v );
//!
//! \brief X,Y步进
//! * 基于当前的相对移动
//! * 如果n>1，执行一步后需要continue
//! \param vi
//! \param x 相对位置
//! \param y 相对位置
//! \param v
//! \param n 重复次数
//! \return
//!
int letStep( int vi, float x, float y, float v, int n=1 );
//!
//! \brief Z轴步进
//! * Z轴基于当前的运动
//! \param vi
//! \param dz
//! * >0 向下移动
//! * <0 向上移动
//! \param v
//! \param tmo
//! \return
//!
int letStepZ( int vi, float dz, float v, int tmo = def_tmo );

//!
//! \brief 扫描过程中进行下一个点的扫描
//! \param vi
//! \return
//! \see letSnakex letSnakey letZigzagx, letZigzagy
//!
int letContinue( int vi );

//!
//! \brief 强制停止
//! \param vi
//! \return
//!
int letEStop( int vi );

//!
//! \brief 恢复默认设置
//! \note 恢复默认设置后，不能撤销，请在调用这个函数前明确之前的设置不再会被使用
//! \param vi
//! \return
//!
int letConfigRst( int vi );

//!
//! \brief 配置工作零位
//! \param vi
//! \param x
//! \param y
//! \param z 固定为0
//!
//! \return
//!
int letConfigOrigin( int vi, float x, float y, float z );

//!
//! \brief 配置方向
//! * 工作空间和默认的坐标方向
//! * 默认坐标方向，起点在右上角
//! * 向左是Y的正向
//! * 向后是X的正向
//! \param vi
//! \param dirx
//! \param diry
//! \return
//!
int letConfigDir( int vi, int dirx, int diry );

//!
//! \brief 配置工作范围
//! \param vi
//! \param w y范围
//! \param h x范围
//! \return
//!
int letConfigWh( int vi, float w, float h );

//!
//! \brief 配置步进数值
//! * 步进数值影响扫描过程的x,y步进值
//! \param vi
//! \param dw y步进
//! \param dh x步进
//! \see letZigzagx letZigzagy letSnakex letSnakey letSlope
//! \return
//!
int letConfigDwDh( int vi, float dw, float dh);

//!
//! \brief 配置扫描中的回扫速度
//! * 一行或一列扫描到最远端后以这个速度运行到下一个行或列
//! \param vi
//! \param rv
//! \return
//! \see letZigzagx letZigzagy letSnakex letSnakey
//!
int letConfigRv( int vi, float rv );

//!
//! \brief 配置Z相关的参数
//! \param vi
//! \param gap 和机械限位的间隙
//! \param gapspeed 运行机械间隙的速度
//! \param zhomespeed Z到机械限位的速度
//! \return
//! \see letHomeZ
//!
int letConfigZGap( int vi, float gap, float gapspeed, float zhomespeed );

//!
//! \brief 查询工作起始位
//! \param vi
//! \param x
//! \param y
//! \param z
//! \return
//! \see letConfigOrigin
//!
int letQueryConfigOrigin( int vi, float &x, float &y, float &z );

//!
//! \brief 查询工作范围
//! \param vi
//! \param w Y范围
//! \param h X范围
//! \param d
//! \return
//!
int letQueryConfigWhz( int vi, float &w, float &h, float &d );

//!
//! \brief 查询方向
//! \param vi
//! \param dirx
//! \param diry
//! \return
//!
int letQueryConfigDir( int vi, int &dirx, int &diry );

//!
//! \brief 查询步进
//! \param vi
//! \param dw
//! \param dh
//! \param dd
//! \return
//!
int letQueryConfigDwDhDd( int vi, float &dw, float &dh, float &dd );

//!
//! \brief 查询回扫速度
//! \param vi
//! \param rv
//! \return
//!
int letQueryConfigRv( int vi, float &rv );

//!
//! \brief 查询Z轴相关参数
//! \param vi
//! \param gap
//! \param gapspeed
//! \param zhomespeed
//! \return
//!
int letQueryConfigGap( int vi, float &gap, float &gapspeed, float &zhomespeed );

//!
//! \brief 查询位置
//! \param vi
//! \param x
//! \param y
//! \param z
//! \return
//!
int letQueryPose( int vi, float &x, float &y, float &z );

//!
//! \brief 查询设备状态
//! \param vi
//! \param stat
//! * "running": 运行中
//! * "stoped"：停止
//! * "pending"：等待
//! * "exception_stoped"：设备异常
//! \return
//!
int letQueryStatus( int vi, std::string &stat );







#endif // LET_API_H
