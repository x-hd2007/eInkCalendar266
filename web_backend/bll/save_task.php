<?php

// 保存工作任务

include_once('config.php');
include_once('db_conn.php');
include_once('todotask_item.php');
include_once('util.php');
include_once('api_result.php');

// 实现数据保存
class save_task
{
    var $m_conn;

    function __construct()
    {
        global $mysql_host;
        global $mysql_user;
        global $mysql_pwd;
        global $mysql_db;

        $this->m_conn = new db_conn($mysql_host,
                $mysql_user, $mysql_pwd, $mysql_db);
    }

    function get_guid()
    {
        return $this->m_conn->get_guid();
    }

    function saveData($task)
    {
        $saveret = false;
        $sql = $task->toSql();
        if (strlen($sql) <= 0) {
            debugln("数据库脚本为空");
            return $saveret;
        }
        $this->m_conn->query($sql);
        $saveret = true;
        return $saveret;
    }

    function updateData($task)
    {
        $saveret = false;
        $sql = $task->toUpdateSql();
        $this->saveDbLog("savetask", $sql);
        if (strlen($sql) <= 0) {
            debugln("数据库脚本为空");
            return $saveret;
        }
        $this->m_conn->query($sql);
        $saveret = true;
        return $saveret;
    }

    function finishTask($tid)
    {
        $sql = "update _tb_todoTask set taskState = 1";
        if (strlen($tid) <= 10)
        {
            $sql = $sql ." where id = ";
            $sql = $sql .strval($tid) .";";
        }
        else {
            $sql = $sql ." where task_guid = '";
            $sql = $sql .$tid ."';";
        }

        debugln($sql);
        $this->m_conn->query($sql);
    }

    function deleteTask($tid)
    {
        $sql = "update _tb_todoTask set isDeleted = 1";
        $sql = $sql ." where id = ";
        $sql = $sql .strval($tid) .";";
        debugln($sql);
        $this->m_conn->query($sql);
    }

    function save($pval, $userid)
    {
        // $sret = new save_result();
        $sret = new api_result();
        if (strlen($pval) <= 0) {
            $sret->code = 205;
            $sret->msg = "推送的数据为空";
            return $sret;
        }

        $taskData = json_decode($pval);
        debugln('数据解析完成');

        if ($taskData == NULL)
        {
            debugln("空白的任务信息");
            $sret->code = 205;
            $sret->msg = "空白的任务数据";
            return $sret;
        }

        $task = new todotask_item();
        $task->copyData($taskData);
        $task->uid = $userid;

        $tguid = $this->get_guid();
        $task->task_guid = $tguid;

        if ($this->saveData($task) == false)
        {
            $sret->code = 206;
            $sret->msg = "保存数据失败";
            return $sret;
        }

        $sret->code = 200;
        // $sret->msg = "保存数据成功";
        $sret->msg = $tguid;

        return $sret;

    }

    function saveDbLog($mtag, $info)
    {
        $linfo = makeAuthLog("debug", $mtag, $info);
        $this->saveAuthLogImpl($linfo);
    }

    function updateTask($pval)
    {
        $sret = new api_result();
        if (strlen($pval) <= 0) 
        {
            $sret = make_api_result(205, "推送的数据为空");
            return $sret;
        }
        $taskData = json_decode($pval);
        debugln('数据解析完成');

        if ($taskData == NULL)
        {
            debugln("空白的任务信息");
            $sret = make_api_result(205, "空白的任务数据");
            return $sret;
        }

        $task = new todotask_item();
        $task->updateTaskData($taskData);

        if ($this->updateData($task) == false)
        {
            $sret = make_api_result(206, "保存数据失败");
            return $sret;
        }

        $sret = make_api_result(200, "保存数据成功");

        return $sret;
    }
    
    function saveAuthLogImpl($authlog)
    {
        $sql = "insert delayed into _tb_authLog (remote_host,";
        $sql = $sql ." module_tag, log_info) values ('";
        $sql = $sql .$authlog->remote_host ."', '";
        $sql = $sql .$authlog->module_tag ."', '";
        $sql = $sql .addslashes($authlog->log_info) ."');";
        
        debugln($sql);
        $this->m_conn->query($sql);
    }

    function saveWeatherInfoImpl($winfo)
    {
        $sql = "insert into _tb_weatherInfo (city_id, weather_type, weather_info) ";
        $sql = $sql ." values ('" . $winfo->city_id ."', '";
        $sql = $sql .$winfo->weather_type ."', '";
        $sql = $sql .addslashes($winfo->weather_info) ."');";

        debugln($sql);
        $this->m_conn->query($sql);
    }

}

function saveWeather($cid, $wtype, $winfo)
{
    $witem = makeWeatherInfo($cid, $wtype, $winfo);
    $st = new save_task();
    $st->saveWeatherInfoImpl($witem);
}

function saveAuthLog($rhost, $mtag, $info)
{
    $linfo = makeAuthLog($rhost, $mtag, $info);
    $st = new save_task();
    $st->saveAuthLogImpl($linfo);
}

function saveOpLog($mtag, $info)
{
    $linfo = makeAuthLog("debug", $mtag, $info);
    $st = new save_task();
    $st->saveAuthLogImpl($linfo);
}

function stUnitTest()
{
    $st = new save_task();
    $task = new todotask_item();
    $st->finishTask(21);
    $st->finishTask(24);
}

function logSaveTest()
{
    $linfo = makeAuthLog("192.168.11.161", "save_task.php", "测试授权验证");
    $st = new save_task();
    $st->saveAuthLog($linfo);
}

function guid_test()
{
    $st   = new save_task();
    $guid = $st->get_guid();
    writeln($guid);
    writeln(strlen($guid));
    
}

// guid_test();

