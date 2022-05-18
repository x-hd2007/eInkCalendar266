<?php

/**
 * 日志信息记录
 */
class log_file
{
    var $m_handle;
    var $m_error;

    function __construct($prefix, $index) {
        // $filename = $prefix . $index .".log";
        $filename = $prefix . $index .".sql";
        $this->m_handle = fopen($filename, "a+");
        if ($this->m_handle === FALSE) {
            $this->m_error = TRUE;
        } else {
            $this->m_error = FALSE;
        }
    }

    function __destruct() {
        if ($this->m_handle != FALSE) {
            fclose($this->m_handle);
        }
    }

    function writeln($info) {
        if ($this->m_error) {
            return;
        }
        $i = $info ."\n";
        fwrite($this->m_handle, $i);
    }

    function write($info) {
        if ($this->m_error) {
            return ;
        }
        fwrite($this->m_handle, $info);
    }

};

