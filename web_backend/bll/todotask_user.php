<?php

// todotask 用户

include_once('config.php');
include_once('db_conn.php');
include_once('util.php');

/**
 * 系统用户信息
 */
class todotask_user
{
    var $id;
    var $userName;
    var $nickName;
    var $accKey;
    var $device_id;
    var $lastVersion;
    var $imgUrl;
    var $enabled;
    var $createTime;
}

