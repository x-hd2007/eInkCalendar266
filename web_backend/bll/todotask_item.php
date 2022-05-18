<?php

// todotask 项目

include_once('config.php');
include_once('db_conn.php');
include_once('util.php');

/**
 * 工作任务
 */
class todotask_item
{
    var $id;
    var $uid;
    var $taskInfo;
    var $taskTag;
    var $category;
    var $priority;
    var $taskRemark;
    var $position;
    var $beginTime;
    var $endTime;
    var $taskState;
    var $task_guid;

    function __construct()
    {
    }

    function makeData()
    {
        $this->taskInfo = "test data";
        $this->taskTag = "test tag";
        $this->category = "test Category";
        $this->priority = 1;
        $this->taskRemark = "test remark";
        $this->beginTime = getCurHour();
        $this->endTime = getNextHour();
        $this->taskState = 0;
    }

    function toSql()
    {
        $sql = "";
        $sql = "insert into _tb_todoTask (uid, taskInfo, taskTag, ";
        $sql = $sql ."category, priority, taskRemark, position, beginTime, ";
        $sql = $sql ."endTime, task_guid) values (";

        $sql = $sql .makeField($this->uid, 1) .", ";
        $sql = $sql .makeField($this->taskInfo, 2) .", ";
        $sql = $sql .makeField($this->taskTag, 2) .", ";
        $sql = $sql .makeField($this->category, 2) .", ";
        $sql = $sql .makeField($this->priority, 1) .", ";
        $sql = $sql .makeField($this->taskTag, 2) .", ";
        $sql = $sql .makeField($this->position, 2) .", ";
        $sql = $sql ."from_unixtime(" .makeField($this->beginTime, 1) ."), ";
        $sql = $sql ."from_unixtime(".makeField($this->endTime, 1) ."), ";
        $sql = $sql .makeField($this->task_guid, 2) .");";
        debugln($sql);
        return $sql;
    }

    function toUpdateSql()
    {
        $sql = "";

        /*
        $sql = "update  _tb_todoTask (taskInfo, taskTag, ";
        $sql = $sql ."category, priority, taskRemark, position, beginTime, ";
        $sql = $sql ."endTime) values (";
        */
        $sql = "update _tb_todoTask ";
        $sql = $sql ."set taskInfo = " .makeField($this->taskInfo, 2) .", ";
        $sql = $sql ."taskTag = " .makeField($this->taskTag, 2) .", ";
        $sql = $sql ."category = " .makeField($this->category, 2) .", ";
        $sql = $sql ."priority = " .makeField($this->priority, 1) ." ";
        // $sql = $sql ."taskRemark = " .makeField($this->taskTag, 2) .", ";
        // $sql = $sql ."position = " .makeField($this->position, 2) .", ";
        // $sql = $sql ."beginTime = " ."from_unixtime(" .makeField($this->beginTime, 1) ."), ";
        // $sql = $sql ."endTime = " ."from_unixtime(".makeField($this->endTime, 1) .")";
        $sql = $sql ." where id = " .strval($this->id) .";";
        debugln($sql);
        return $sql;
    }

    function copyData($data)
    {
        $this->taskInfo = $data->taskInfo;
        $this->taskTag = $data->taskTag;
        $this->category = $data->category;
        $this->priority = $data->priority;
        $this->position = $data->position;
        $this->beginTime = getCurHour();
        $this->endTime = getNextHour();
    }

    function updateTaskData($data)
    {
        $this->id = $data->id;
        $this->taskInfo = $data->taskInfo;
        $this->taskTag = $data->taskTag;
        $this->category = $data->category;
        $this->priority = $data->priority;
        $this->position = $data->position;
        $this->beginTime = getCurHour();
        $this->endTime = getNextHour();
    }

}

class devicetask_item
{
    var $ti;
    var $tt;
    var $pri;
    var $bt;
    var $et;

    function copyData($data)
    {
        $this->ti = $data->taskInfo;
        $this->tt = $data->taskTag;
        $this->pri = $data->priority;
        $this->bt = $data->beginTime;
        $this->et = $data->endTime;
    }
}

/** 任务分类
 */
class todotask_category
{
    var $category;
    var $count;
}

/** 任务标签
 */
class todotask_tag
{
    var $taskTag;
    var $count;
}

/** 权限验证记录
 */
class auth_log
{
    var $remote_host;
    var $module_tag;
    var $log_info;
}

class weather_info
{
    var $city_id;
    var $weather_type;
    var $weather_info;
}

/**
 * 手持 app 更新信息
 */
class app_update_info
{
    var $ver_num;
    var $url;
}

/**
 * 生成授权日志
 */
function makeAuthLog($rhost, $mtag, $alog)
{
    $linfo = new auth_log();
    $linfo->remote_host = $rhost;
    $linfo->module_tag = $mtag;
    $linfo->log_info = $alog;
    return $linfo;
}

function makeWeatherInfo($cid, $wtype, $winfo)
{
    $witem = new weather_info();
    $witem->city_id = $cid;
    $witem->weather_type = $wtype;
    $witem->weather_info = $winfo;
    return $witem;
}

