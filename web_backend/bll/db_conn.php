<?php

include_once('config.php');
include_once('util.php');

//数据库链接 FOR PHP > 7.0
class mysql_conn_impl
{
    var $m_conn;        // 数据库链接
    var $m_db_name;     // 数据库名称

    function __construct($host, $user, $pwd, $db_name)
    {
        $this->m_conn = new mysqli($host, $user, $pwd, $db_name);
        if ($this->m_conn->connect_errno)
        {
            writeln("Failed to connect to MYSQL! Error: " . $this->m_conn->connect_error);
        }
        else
        {
            // writeln("MYSQL CONNECT OK!");
            $this->m_db_name = $db_name;
            $this->m_conn->query("set names utf8");
        }
    }

    function close()
    {
        debug_line(__FILE__ . " : " .__LINE__ ." 关闭数据库连接");
        $this->m_conn->close();
    }

    function num_rows($result) 
    {
        return mysqli_num_rows($result);
    }

    function fetch_row($result)
    {
        return mysqli_fetch_row($result);
    }

    function free_result($result)
    {
        return mysqli_free_result($result);
    }
    
    function __destruct()
    {
        //$this->m_conn->close();
    }

    function query($sql) {
        return $this->m_conn->query($sql);
    }

}

//数据库链接
class db_conn_impl {

    var $m_conn;        //数据库连接
    var $m_db_name;     //爬虫数据库名称

    /* 构造函数
    */
    function __construct($host, $user, $pwd, $db_name) {
        $this->m_conn = mysql_connect($host, $user, $pwd);
        if (!$this->m_conn) die("error: mysql connect failed!" .$hosti ."\n");
        mysql_query("set names utf8");
        $this->m_db_name = $db_name;
    }

    /* 析构函数
    */
    function __destruct() {
        //mysql_close($this->m_conn);
    }

    function num_rows($result) 
    {
        return mysql_num_rows($result);
    }

    function fetch_row($result) 
    {
        return mysql_fetch_row($result);
    }

    function free_result($result) 
    {
        return mysql_free_result($result);
    }
    
    /* 执行查询语句
    */
    function query($sql) {
        mysql_select_db($this->m_db_name, $this->m_conn);
        return mysql_query($sql, $this->m_conn);
    }
}

//数据库链接
class db_conn
{

    var $m_connImpl;        //数据库连接
    var $m_db_name;     //爬虫数据库名称

    /* 构造函数
    */
    function __construct($host, $user, $pwd, $db_name) {

        global $g_php7;
        if ($g_php7 == 1) 
        {
            $this->m_connImpl = new mysql_conn_impl($host, $user,
                                                    $pwd, $db_name);
        }
        else 
        {
            $this->m_connImpl = new db_conn_impl($host, $user,
                                                 $pwd, $db_name);
        }        
    }

    /* 析构函数
    */
    function __destruct() {
    }

    function num_rows($result) 
    {
        return $this->m_connImpl->num_rows($result);
    }

    function fetch_row($result) 
    {
        return $this->m_connImpl->fetch_row($result);
    }

    function free_result($result) 
    {
        return $this->m_connImpl->free_result($result);
    }

    /* 执行查询语句
    */
    function query($sql) {
        return $this->m_connImpl->query($sql);
    }

    function get_guid()
    {
        $ret = "";
        $sql = "select uuid() as uid;";
        $result = $this->query($sql);
        if ($this->num_rows($result) > 0)
        {
            $row = $this->fetch_row($result);
            $ret = $row[0];
        }
        $this->free_result($result);
        return $ret;
    }

}
