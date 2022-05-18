<?php

// 加载任务 分类

include_once('config.php');
include_once('./bll/util.php');
include_once('./bll/auth_check.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');
include_once('./bll/loadTodoTask.php');

function category_main()
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
        saveAuthLog($_SERVER['REMOTE_ADDR'], "category.php", "用户验证失败");

        $einfo = make_error_404();
        
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }
    else 
    {
        saveAuthLog($_SERVER['REMOTE_ADDR'], "category.php", $uid ." - 查看所有项目分类");
        header('Content-Type:application/json; charset=utf-8');
        load_todotask_category($uid);
        // load_todotask_category($uid);
    }
}

category_main();

