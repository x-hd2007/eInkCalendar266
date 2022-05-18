<?php

// 工具函数

// include_once('../config.php');
include_once('config.php');

date_default_timezone_set("PRC");

function writeln($info)
{
    echo($info ."\n");
}

function writearray($alist)
{
    foreach($alist as $aval)
    {
        echo ($aval ." ");
    }
    echo("\n");
}

function debugln($info)
{
    global $g_debug;
    if ($g_debug == 1) 
    {
        echo($info ."\n");
    }
}

function isDebug()
{
    global $g_debug;
    if ($g_debug == 1) 
    {
        return true;
    }
    return false;
}

function makeStr($index, $len)
{
    $ret = strval($index);
    while(strlen($ret) < $len) 
    {
        $ret = "0" .$ret;
    }
    return $ret;
}

function getTimestamp()
{
    return time();
}

function getCurHour()
{
    $ntime = time();
    $msecond = ($ntime % 3600);
    $ntime = $ntime - $msecond;
    return $ntime;
}

function getHourStr()
{
    return date("H");
}

function getNextHour()
{
    return getCurHour() + 3600;
}

function getCurYear()
{
    return date("Y");
}

function getCurMonth()
{
    return date("n");
}

function getCurDay()
{
    return date("j");
}

function makeField($val, $ftype)
{
    $ret = "";
    if ($ftype == 1)
    {
        $ret = strval($val);
    }
    else if ($ftype == 2)
    {
        $ret = "'" .$val ."'";
    }
    else if ($ftype == 3)
    {
        $ret = "'" .$val ."'";
    }
    return $ret;
}

function utUnitTest()
{
    writeln(makeStr(10, 4));
    writeln(makeStr(1, 5));
    writeln(makeStr(10000, 3));
}

function my_json_encode($array)
{
    if(version_compare(PHP_VERSION,'5.4.0','<')){
        $str = json_encode($array);
        $str = preg_replace_callback("#\\\u([0-9a-f]{4})#i",function($matchs){
                return iconv('UCS-2BE', 'UTF-8', pack('H4', $matchs[1]));
        },$str);
        return $str;
    }
    else{
        return json_encode($array, JSON_UNESCAPED_UNICODE);
    }
}

function calc_device_ack($prefix, $devid)
{
    $ret = md5($prefix .getHourStr() .$devid);
    return $ret;
}

function time_ak_test()
{
    writeln(getHourStr());
    writeln(calc_device_ack("eInkCalendar", "V12021_0001"));
}

function g_time_test()
{
    writeln(getCurYear() ." " .getCurMonth() . " " .getCurDay());
}

/*
function get_guid()
{
    if (function_exists('com_create_guid') === true) 
    {
        return trim(com_create_guid(), '{}');
    }
    return "guid function error";
}
*/

// time_ak_test();


