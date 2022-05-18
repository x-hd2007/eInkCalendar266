<?php

// 加载工作任务实现

include_once('loadTools.php');
include_once('todotask_list.php');
include_once('todotask_user.php');
include_once('lunar.php');

/**
 * 加载设备显示任务
 */
function load_short_todotask($userName, $userid)
{
    $lt = new loadTools();
    $tlist = new todotask_list();
    $lt->loadShortTask($userName, $userid, $tlist);
    $tlist->lYear = get_lunar_year_info();
    $tlist->lDay = get_lunar_day_info();
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

/**
 * 获取设备显示任务
 */
function load_device_todotask($userName, $userid)
{
    $lt = new loadTools();
    $tlist = new todotask_list();
    $lt->loadShortTask($userName, $userid, $tlist);
    $dlist = array();
    foreach($tlist->task_list as $titem)
    {
        $ditem = new devicetask_item();
        $ditem->copyData($titem);
        $dlist[] = $ditem;
    }
    $tlist->task_list = $dlist;
    $tlist->lYear = get_lunar_year_info();
    $tlist->lDay = get_lunar_day_info();
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

/**
 * 获取所有代办任务
 */
function load_all_todotask($userName, $userid)
{
    $lt = new loadTools();
    $tlist = new todotask_list();
    $lt->loadAllTask($userName, $userid, $tlist);

    $uinfo = new todotask_user();
    $lt->loadTaskUser($userName, $uinfo);
    $tlist->imgUrl = $uinfo->imgUrl;
    $tlist->lYear = get_lunar_year_info();
    $tlist->lDay = get_lunar_day_info();
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

/**
 * 获取所有已完成任务
 */
function load_all_donetask($userName, $userid)
{
    $lt = new loadTools();
    $tlist = new todotask_list();
    $lt->loadAllDoneTask($userName, $userid, $tlist);
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

/**
 * 加载用户任务
 */
function load_todotask_by_id($userName, $tid)
{
    $lt = new loadTools();
    $tlist = new todotask_list();
    $lt->loadTaskInfo($userName, $tid, $tlist);
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

/**
 * 加载任务分类,项目
 */
function load_todotask_category($userName)
{
    $lt = new loadTools();
    $uinfo = new todotask_user();
    $lt->loadTaskUser($userName, $uinfo);
    $clist;
    $lt->loadTaskCategory($uinfo->id, $clist);
    $jstr = my_json_encode($clist);
    writeln($jstr);
}


/**
 * 加载任务标签
 */
function load_todotask_tag($userName)
{
    $lt = new loadTools();
    $uinfo = new todotask_user();
    $lt->loadTaskUser($userName, $uinfo);
    $tlist;
    $lt->loadTaskTags($uinfo->id, $tlist);
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

/**
 * 加载用户标签
 */
function load_user_tag($userName)
{
    $lt = new loadTools();
    $uinfo = new todotask_user();
    $lt->loadTaskUser($userName, $uinfo);
    $tlist;
    $lt->loadUserTags($uinfo->id, $tlist);
    $jstr = my_json_encode($tlist);
    writeln($jstr);
}

// load_todotask_by_id("xuhaidong", 19);

