<?php

// 字符串数据

include_once('config.php');
include_once('db_conn.php');
include_once('util.php');

/**
 * 字符串列表
 */
class StringArray
{
    var $m_strArray;
    
    function __construct()
    {
        $this->m_strArray = array();
    }

    function add($info) {
        $this->m_strArray[] = $info;
    }

    function toString() {
        $ret = "";
        foreach($this->m_strArray as $val) {
            $ret = $ret .$val ."\n";
        }
        return $ret;
    }

    function size() {
        return count($this->m_strArray);
    }
}

function strUnitTest()
{
    $sa = new StringArray();
    $sa->add("hello world");
    $sa->add("adsfasdfasdfasd");
    writeln($sa->size());
    writeln($sa->toString());
}

// strUnitTest();

