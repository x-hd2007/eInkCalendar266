<?php

// 检查 app 更新版本信息

include_once('./bll/loadTodoTask.php');
include_once('./bll/auth_check.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');
include_once('./bll/todotask_item.php');

function appupdate_main()
{
    $uid = $_GET["uid"];
    $acckey = $_GET["ak"];

    $einfo = "";

    if (!isset($uid) || $uid == "")
    {
        $einfo = make_error_404();
    }

    if (!isset($acckey) || $acckey == "")
    {
        $einfo = make_error_404();
    }

    $auth = checkAuth($uid, $acckey);

    debugln("check result: " .$auth);

    if ($auth <= 0)
    {
        // 记录错误授权日志
        saveAuthLog($_SERVER['REMOTE_ADDR'], "appupdate.php", "用户验证失败");

        $einfo = make_error_404();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return;
    }
    else
    {
        saveAuthLog($_SERVER['REMOTE_ADDR'], "appupdate.php", $uid ." - 检查手机端程序更新");
        
        header('Content-Type:application/json; charset=utf-8');
        $upinfo = new app_update_info();
        $upinfo->ver_num = 3600;
        $upinfo->url = "http://news.sina.com.cn/";
        writeln(my_json_encode($upinfo));
    }

}

appupdate_main();
