<?php

// api 借口结果

class api_result
{
    var $code;
    var $msg;    
}

/**
 * 生成接口结果
 * @param [in] 结果编号
 * @param [in] 返回消息
 */
function make_api_result($code, $msg)
{
    $ret = new api_result();
    $ret->code = $code;
    $ret->msg = $msg;
    return $ret;
}

/**
 * 返回 404 错误
 */
function make_error_404()
{
    return make_api_result(404, "访问被禁止");
}

/**
 * 返回 500 错误
 */
function make_error_500()
{
    return make_api_result(500, "服务器内部错误");
}

/**
 * 返回 200 结果
 */
function make_ok_200()
{
    return make_api_result(200, "操作成功");
}

