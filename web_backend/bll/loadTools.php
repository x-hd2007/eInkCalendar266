<?php

// 从数据库加载数据

include_once('config.php');
include_once('util.php');
include_once('db_conn.php');

/**
 * 数据加载实现类
 */
class loadTools
{
    var $m_conn;

    function __construct()
    {
        global $mysql_host;
        global $mysql_user;
        global $mysql_pwd;
        global $mysql_db;

        $this->m_conn = new db_conn($mysql_host, $mysql_user, $mysql_pwd, $mysql_db);
    }

    /**
     * 加载工作实现
     * @param [in]
     * @param [in]
     * @param [in]
     */
    function loadTaskImpl($sql, &$tlist)
    {
        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);

        if ($num_row <= 0) 
        {
            debugln("任务数据为空!");
            $this->m_conn->free_result($result);
            $tlist->task_count = 0;
            $tlist->version = 1;
            return;
        }

        $tlist->task_list = array();
        $row = $this->m_conn->fetch_row($result);
        while ($row)
        {
            $titem = new todotask_item();
            $titem->id = $row[0];
            $titem->taskInfo = $row[1];
            $titem->taskTag = $row[2];
            $titem->category = $row[3];
            $titem->priority   = $row[4];
            $titem->taskRemark   = $row[5];
            $titem->beginTime   = $row[6];
            $titem->endTime   = $row[7];
            $titem->taskState   = $row[8];
            $titem->uid = $row[9];
            $titem->position = $row[10];

            $tlist->task_list[] = $titem;
            $row = $this->m_conn->fetch_row($result);
            
        }
        $this->m_conn->free_result($result);

        $tlist->task_count = count($tlist->task_list);
        $tlist->version = 1;
        // $tlist->userName = $userName;
    }

    /**
     * 加载所有工作
     * @param [in] userName
     * @param [in] userid
     * @param [in] tlist
     */
    function loadAllTask($userName, $userid, &$tlist)
    {
        $sql = "select id, taskInfo, taskTag, category, priority,";
        $sql = $sql ." taskRemark, unix_timestamp(beginTime), unix_timestamp(endTime), taskState";
        $sql = $sql . " ,uid, position ";
        $sql = $sql ." from _tb_todoTask where taskState <= 0 and isDeleted = 0";
        $sql = $sql ." and uid = " .strval($userid);
        $sql = $sql ." order by priority asc, id desc";

        $this->loadTaskImpl($sql, $tlist);
        $tlist->userName = $userName;
    }

    /**
     * 加载所有完成工作
     * @param [in] userName
     * @param [in] userid
     * @param [in] tlist
     */
    function loadAllDoneTask($userName, $userid, &$tlist)
    {
        $sql = "select id, taskInfo, taskTag, category, priority,";
        $sql = $sql ." taskRemark, unix_timestamp(beginTime), unix_timestamp(endTime), taskState";
        $sql = $sql ." from _tb_todoTask where taskState > 0 and isDeleted = 0";
        $sql = $sql ." and uid = " .strval($userid);
        $sql = $sql ." order by priority asc, id desc";

        $this->loadTaskImpl($sql, $tlist);
        $tlist->userName = $userName;
    }

    /**
     * 获取设备显示任务列表
     * @param [in] userName 
     * @param [in] userid
     * @param [out] tlist
     */
    function loadShortTask($userName, $userid, &$tlist)
    {
        $sql = "select id, taskInfo, taskTag, category, priority,";
        $sql = $sql ." taskRemark, unix_timestamp(beginTime), unix_timestamp(endTime), taskState";
        $sql = $sql ." ,uid, position ";
        $sql = $sql ." from _tb_todoTask where taskState <= 0 and isDeleted = 0";
        $sql = $sql ." and uid = " .strval($userid);
        $sql = $sql ." order by priority asc, id desc limit 0, 9";

        $this->loadTaskImpl($sql, $tlist);
        $tlist->userName = $userName;
    }

    /**
     * 加载代办任务
     * @param [in] userName 用户名称
     * @param [in] tid 用户编号
     * @param [out] tlist 任务列表
     */
    function loadTaskInfo($userName, $tid, &$tlist)
    {
        $sql = "select id, taskInfo, taskTag, category, priority,";
        $sql = $sql ." taskRemark, unix_timestamp(beginTime), unix_timestamp(endTime), taskState";
        $sql = $sql ." from _tb_todoTask where id = ";
        $sql = $sql .strval($tid) ." ;";

        // debugln($sql);

        $this->loadTaskImpl($sql, $tlist);
        $tlist->userName = $userName;
    }

    /**
     * 加载用户信息
     * @param [in] userName 用户名称
     * @param [out] tinfo 用户信息
     */
    function loadTaskUser($userName, &$tinfo)
    {
        $sql = "select id, userName, accKey, lastVersion, unix_timestamp(createTime), imgUrl, enabled, nickName";
        $sql = $sql . " from _tb_taskUser where userName = '";
        $sql = $sql . $userName . "';";

        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);
        if ($num_row <= 0) 
        {
            debugln("用户数据为空");
            $this->m_conn->free_result($result);
            return ;
        }

        $row = $this->m_conn->fetch_row($result);
        $tinfo->id = $row[0];
        $tinfo->userName = $row[1];
        $tinfo->accKey = $row[2];
        $tinfo->lastVersion = $row[3];
        $tinfo->createTime = $row[4];
        $tinfo->imgUrl = $row[5];
        $tinfo->enabled = $row[6];
        $tinfo->nickName = $row[7];

        $this->m_conn->free_result($result);
        
    }

    //  根据设备号加载用户信息
    function loadDeviceUser($devid, &$tinfo)
    {
        $sql = "select id, userName, accKey, device_id, lastVersion, unix_timestamp(createTime), nickName";
        $sql = $sql . " from _tb_taskUser where device_id like '%";
        $sql = $sql . $devid . "%';";

        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);
        if ($num_row <= 0) 
        {
            debugln("用户数据为空");
            $this->m_conn->free_result($result);
            return ;
        }

        $row = $this->m_conn->fetch_row($result);
        $tinfo->id = $row[0];
        $tinfo->userName = $row[1];
        $tinfo->accKey = $row[2];
        $tinfo->device_id = $row[3];
        $tinfo->lastVersion = $row[4];
        $tinfo->createTime = $row[5];
        $tinfo->nickName = $row[6];

        $this->m_conn->free_result($result);

    }

    // 加载任务分类
    function loadTaskCategory($uid, &$category_list)
    {
        $sql = "select category, count(category) as count from _tb_todoTask where ";
        $sql = $sql ." uid = " .strval($uid);
        $sql = $sql ." group by category order by count desc;";

        debugln($sql);

        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);
        if ($num_row <= 0)
        {
            debugln("记录数量为空！");
            $this->m_conn->free_result($result);
            return;
        }

        $category_list = array();
        $row = $this->m_conn->fetch_row($result);
        while ($row)
        {
            $citem = new todotask_category();
            $citem->category = $row[0];
            $citem->count = $row[1];
            if (strlen($citem->category) > 0)
            {
                $category_list[] = $citem;
            }
            $row = $this->m_conn->fetch_row($result);
        }
        $this->m_conn->free_result($result);
    }

    // 加载任务表标签数据
    function loadTaskTags($uid, &$tag_list)
    {
        $sql = "select taskTag, count(taskTag) as count from _tb_todoTask where ";
        $sql = $sql ." uid = " .strval(uid);
        $sql = $sql ." group by taskTag order by count desc;";

        debugln($sql);

        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);
        if ($num_row <= 0)
        {
            debugln("记录数量为空！");
            $this->m_conn->free_result($result);
            return;
        }

        $tag_list = array();
        $row = $this->m_conn->fetch_row($result);
        while ($row)
        {
            $titem = new todotask_tag();
            $titem->taskTag = $row[0];
            $titem->count = $row[1];
            if (strlen($titem->taskTag) > 0)
            {
                $tag_list[] = $titem;
            }
            $row = $this->m_conn->fetch_row($result);
        }
        $this->m_conn->free_result($result);
    }

    // 加载用户标签表数据
    function loadUserTags($uid, &$tag_list)
    {
        $sql = "select taskName, count(taskName) as count from _tb_userTag where ";
        $sql = $sql ." uid = " .strval(uid);
        $sql = $sql ." group by taskName order by count desc;";

        debugln($sql);

        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);
        if ($num_row <= 0)
        {
            debugln("记录数量为空！");
            $this->m_conn->free_result($result);
            return;
        }

        $tag_list = array();
        $row = $this->m_conn->fetch_row($result);
        while ($row)
        {
            $titem = new todotask_tag();
            $titem->taskTag = $row[0];
            $titem->count = $row[1];
            if (strlen($titem->taskTag) > 0)
            {
                $tag_list[] = $titem;
            }
            $row = $this->m_conn->fetch_row($result);
        }
        $this->m_conn->free_result($result);
    }

    // 加载最新的天气内容
    function loadWeatherInfo($wtype, $cid)
    {
        $ret = "";
        $sql = "select city_id, weather_type, weather_info, unix_timestamp(createTime)";
        $sql = $sql ." from _tb_weatherInfo where city_id='" .$cid;
        $sql = $sql ."' and weather_type='" .$wtype ."' order by id desc limit 0, 2";

        debugln($sql);

        $result = $this->m_conn->query($sql);
        $num_row = $this->m_conn->num_rows($result);
        if ($num_row <= 0)
        {
            debugln("记录数量为空");
            $this->m_conn->free_result($result);
            return $ret;
        }

        $row = $this->m_conn->fetch_row($result);
        if ($row)
        {
            // 取得天气创建时间
            $ts = $row[3];
            $now_ts = getTimestamp();
            $offset = $now_ts - $ts;
            if ($offset <= 7200)
            {
                debugln("小于 2 小时, 天气缓存有效");
                $ret = $row[2];
            }
            else {
                debugln("缓存超过 2 小时, 需重新获取!");
            }
        }
        $this->m_conn->free_result($result);
        return $ret;
    }

}
