<?php


// 执行工作任务完成

include_once('config.php');
include_once('./bll/util.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');
include_once('./bll/auth_check.php');

// 完成工作任务
function doneTask($tid)
{
    $stask = new save_task();
    return $stask->finishTask($tid);
}

function ftask_main()
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
        saveAuthLog($_SERVER['REMOTE_ADDR'], "ftask.php", "用户验证失败");

        $einfo = make_error_404();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }

    $taskid = $_GET["tid"];
    if (!isset($taskid) || $taskid == "")
    {
        // writeln("no task id");
        $einfo = make_error_500();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }

    saveAuthLog($_SERVER['REMOTE_ADDR'], "ftask.php", $uid ." - 完成工作任务 " .$taskid);

    // 完成工作任务

    doneTask($taskid);
    $einfo = make_ok_200();
    header('Content-Type:application/json; charset=utf-8');
    writeln(my_json_encode($einfo));

}

ftask_main();

