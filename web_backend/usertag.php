<?php

// 加载用户标签

include_once('config.php');
include_once('./bll/util.php');
include_once('./bll/auth_check.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');
include_once('./bll/loadTodoTask.php');

/**
 * 获取用户标签表数据
 */
function userTag_main()
{
    $uid = $_GET["uid"];
    $accKey = $_GET["ak"];

    $einfo = "";

    if (!isset($uid) || $uid == "")
    {
        $einfo = make_error_404();
    }

    if (!isset($acckey) || $acckey == "")
    {
        $einfo = make_error_404();
    }

    $auth = checkAuth($uid ,$acckey);

    debugln("check result: " .$auth);

    $auth = 1;

    if ($auth <= 0)
    {
        // 记录错误授权日志
        saveAuthLog($_SERVER['REMOTE_ADDR'], "tasktag.php", "用户验证失败");

        $einfo = make_error_404();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }
    else 
    {
        saveAuthLog($_SERVER['REMOTE_ADDR'], "tasktag.php", $uid ." - 获取任务标签");
        header('Content-Type:application/json; charset=utf-8');
        // load_todotask_tag("xuhaidong");
        load_user_tag($uid);
    }

}

userTag_main();

