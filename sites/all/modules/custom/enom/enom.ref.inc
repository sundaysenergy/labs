<?php

/**
 * @file
 * 
 * This file contains only code for references
 * Enom API implementation is now based on wsclient
 */

/**
 * @param $command - text - enom command, example "check"
 * @param $args - array of args to send to command
 */
function enom_interface_command($command, $args = array()) {
  $args += array(
    'uid'     => variable_get('enom_user', 'sundaysenergy'),
    'pw'      => variable_get('enom_pass', 'zola322'),
    'command' => $command,
  );

  if (empty($args['uid']) || empty($args['pw']) || empty($args['command'])) {
    //@TODO
    return false;
  }

  $enomhost = variable_get('enom_host', 'https://resellertest.enom.com/interface.asp');

  $options = array(
    'query'     => $args,
    'external'  => TRUE,
    'https'     => TRUE,
  );

  $url = url($enomhost, $options);

  $request = drupal_http_request($url, $options);
  if (FALSE == $request) {
    watchdog('enom', t('Could not connect to ENOM server. Please try again later.'), WATCHDOG_ERROR);
    return FALSE;
  }
  if (isset($request->data) && $request->data && $request->code < 400) {
    $result = _enom_parse_response($request->data);
    return $result;
  }
  return $request;
}

/**
 *
 */
function _enom_parse_response($data) {
  //$parse = new 
  return $data;
}
