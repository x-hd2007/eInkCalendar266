<?php

// 加载电子日历中显示任务

include_once('./bll/loadTodoTask.php');
include_once('./bll/auth_check.php');
include_once('./bll/save_task.php');
include_once('./bll/api_result.php');

function devtask_main()
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

    // $auth = checkAuth($uid ,$acckey);

    // 根据设备编号校验访问密钥
    $auth = checkDeviceAuth($uid ,$acckey);

    debugln("check result: " .$auth);

    if ($auth <= 0)
    {
        // 记录错误授权日志
        saveAuthLog($_SERVER['REMOTE_ADDR'], "devtask.php", "用户验证失败");
        
        $einfo = make_error_404();
        
        header('Content-Type:application/json; charset=utf-8');
        writeln(my_json_encode($einfo));
        return ;
    }
    else {

        saveAuthLog($_SERVER['REMOTE_ADDR'], "devtask.php", $uid ." - 设备获取任务信息");

        header('Content-Type:application/json; charset=utf-8');
        // load_device_todotask($uid, $auth);

        $nick = loadUserNickName($uid);
        load_device_todotask($nick, $auth);
    }

}

devtask_main();

