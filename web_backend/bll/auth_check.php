<?php

// 用户权限检查

include_once('config.php');
include_once('util.php');
include_once('loadTools.php');
include_once('todotask_user.php');

/**
 * 检查用户授权
 */
function checkAuth($userName, $accKey)
{
    $ret = 0;

    if (!isset($userName) || $userName == "")
    {
        return $ret;
    }

    if (!isset($accKey) || $accKey == "")
    {
        return $ret;
    }
    
    $lt = new loadTools();
    $tinfo = new todotask_user();
    $lt->loadTaskUser($userName, $tinfo);

    debugln(my_json_encode($tinfo));

    if (strcmp($accKey, $tinfo->accKey) === 0)
    {
        // $ret = 1;
        // 返回用户编号
        $ret = $tinfo->id;
    }

    return $ret;
}

/**
 * 获取昵称
 */
function loadUserNickName($devid)
{
    $ret = "";
    if (!isset($devid) || $devid == "") 
    {
        return $ret;
    }

    $lt = new loadTools();
    $tinfo = new todotask_user();
    $lt->loadDeviceUser($devid, $tinfo);
    if ($tinfo->id <= 0)
    {
        // 设备未绑定用户
        return $ret;
    }
    $ret = $tinfo->nickName;
    return $ret;
}

/**
 * 检查设备授权
 * @param [in] devid 设备编号
 * @param [in] accKey 访问编号
 * @return 成功返回1, 失败返回 0
 */
function checkDeviceAuth($devid, $accKey)
{
    $ret = 0;
    if (!isset($devid) || $devid == "") 
    {
        return $ret;
    }

    if (!isset($accKey) || $accKey == "")
    {
        return $ret;
    }

    $lt = new loadTools();
    $tinfo = new todotask_user();
    $lt->loadDeviceUser($devid, $tinfo);
    if ($tinfo->id <= 0)
    {
        // 设备未绑定用户
        return $ret;
    }
    global $g_device_prefix;

    // 根据时间计算访问 key
    $calc_key = calc_device_ack($g_device_prefix, $devid);

    debugln($calc_key);
    debugln($accKey);

    if (strcmp($calc_key, $accKey) == 0)
    {
        $ret = $tinfo->id;
    }
    return $ret;
}

/**
 * 单元测试代码
 */
function _auth_unit_test()
{
    $ret = checkAuth("xuhaidong", "helloworld");
    debugln("auth check result: " .$ret);
}

// _auth_unit_test();

