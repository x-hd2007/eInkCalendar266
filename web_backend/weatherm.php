<?php

include_once('config.php');
include_once('./bll/util.php');
include_once('./bll/auth_check.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');
include_once('./bll/loadTools.php');

function get_cache_weather($wtype, $cid)
{
    $lt = new loadTools();
    $weather = $lt->loadWeatherInfo($wtype, $cid);
    return $weather;
}

function get_weather($cityid)
{
    $wcache = get_cache_weather("3d", $cityid);
    if (strlen($wcache) > 0)
    {
        writeln($wcache);
        return ;
    }

    // 和风天气访问key
    $wkey = "请输入申请的和风天气 KEY";

    $durl = "https://devapi.qweather.com/v7/weather/3d?gzip=n&location=";
    $durl = $durl .$cityid;
    $durl = $durl ."&key=" .$wkey ."&lang=zh&unit=m";
    $content = file_get_contents($durl);
    saveWeather($cityid, "3d", $content);
    writeln($content);
}

function weather_main()
{
    $ak = $_GET["ak"];
    $cid = $_GET["cid"];

    $einfo = "";

    if (!isset($ak) || !isset($cid) ||
        $ak == "" || $cid == "")
    {
        saveAuthLog($_SERVER['REMOTE_ADDR'], "weatherm.php", " - 错误的请求参数");
        $einfo = make_error_500();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }

    if (strcmp($ak, "hello1234554321") !== 0)
    {
        saveAuthLog($_SERVER['REMOTE_ADDR'], "weatherm.php", " - 错误的访问密码");
        $einfo = make_error_500();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }

    saveAuthLog($_SERVER['REMOTE_ADDR'], "weatherm.php", "获取 3 日天气预报");
    header('Content-Type:application/json; charset=utf-8');
    get_weather($cid);
}

weather_main();

// get_weather("101020200");

