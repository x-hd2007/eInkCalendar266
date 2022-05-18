<?php

include_once('config.php');
include_once('./bll/util.php');
include_once('./bll/save_task.php');
include_once('./bll/auth_check.php');
include_once('./bll/api_result.php');

// 上传工作任务保存
function saveTask($pval, $userid)
{
    $stask = new save_task();
    return $stask->save($pval, $userid);
}

function uploadtask_main()
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
        saveAuthLog($_SERVER['REMOTE_ADDR'], "uploadtask.php", "用户验证失败");

        $einfo = make_error_404();
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return;
    }

    saveAuthLog($_SERVER['REMOTE_ADDR'], "uploadtask.php", $uid ." - 新增工作任务！");

    $pval = file_get_contents("php://input");

    $sret;

    if ($pval != NULL)
    {
        $sret = saveTask($pval, $auth);
    }
    else {
        $ret = make_error_500();
    }

    $jstr = my_json_encode($sret);
    header('Content-Type:application/json; charset=utf-8');
    writeln($jstr);

}

uploadtask_main();

/**
function ut_uploadtask()
{
    $pval = "{ \"taskInfo\": \"hello\", \"taskTag\": \"tag\", \"category\": \"cat\", \"priority\": 1, \"position\": \"shanghai\" }";

    $ret = saveTask($pval, 3);
    $jstr = my_json_encode($ret);
    writeln($jstr);
}

ut_uploadtask();
*/

