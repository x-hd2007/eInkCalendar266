<?php

// 工作任务清单

include_once('config.php');
include_once('db_conn.php');
include_once('util.php');
include_once('todotask_item.php');

/**
 * 代办工作列表
 */
class todotask_list
{
    var $userName;
    var $imgUrl;
    var $version;
    var $lYear;
    var $lDay;
    var $task_count;
    var $task_list;
}

