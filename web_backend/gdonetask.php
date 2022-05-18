<?php

// 获取所有已经完成工作任务

include_once('config.php');
include_once('./bll/loadTodoTask.php');
include_once('./bll/auth_check.php');
include_once('./bll/api_result.php');
include_once('./bll/save_task.php');

function gdonetask_main()
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
        saveAuthLog($_SERVER['REMOTE_ADDR'], "gdonetask.php", "用户验证失败");

        $einfo = make_error_404();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return;
    }
    else
    {
        saveAuthLog($_SERVER['REMOTE_ADDR'], "gdonetask.php", $uid ." - 获取完成工作任务");

        header('Content-Type:application/json; charset=utf-8');
        load_all_donetask($uid, $auth);
    }
}

gdonetask_main();

