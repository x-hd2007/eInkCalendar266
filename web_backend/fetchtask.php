<?php

// 加载执行的工作任务

include_once('./bll/loadTodoTask.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');
include_once('./bll/auth_check.php');

function fetchtask_main()
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


    $auth = checkAuth($uid ,$acckey);

    debugln("check result: " .$auth);

    if ($auth <= 0)
    {
        // 记录错误授权日志
        saveAuthLog($_SERVER['REMOTE_ADDR'], "fetchtask.php", "用户验证失败");

        $einfo = make_error_404();
        
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }

    $taskid = $_GET["tid"];
    if (!isset($taskid) || $taskid == "")
    {
        $einfo = make_api_result(500, "参数错误");
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }

    saveAuthLog($_SERVER['REMOTE_ADDR'], "fetchtask.php", $uid ." - 查看工作任务信息" .$taskid);

    header('Content-Type:application/json; charset=utf-8');
    load_todotask_by_id($uid, $taskid);
}

fetchtask_main();

