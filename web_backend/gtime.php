<?php

include_once('config.php');
include_once('./bll/util.php');

function gtime_main()
{
    $ntime = time();
    writeln($ntime);
}

gtime_main();

