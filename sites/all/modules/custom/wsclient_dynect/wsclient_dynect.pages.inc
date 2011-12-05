<?php

/**
 * @file
 * List of page
 */

function wsclient_dynect_zone() {
  $service = wsclient_service_load('dynect');
  
  $all_zone = $service->get_all_zone();
  $domains = array();
  if(isset($all_zone['status']) && $all_zone['status'] == 'success') {
    foreach($all_zone['data'] as $domain) {
      $lists = explode('/', $domain);
      $name = $lists[3];
      $domains[] = l($name, 'dynect/zone/' . $name);
    }
  } 
  return theme('item_list', array('items' => $domains));
}

function wsclient_dynect_zone_detail($zone) {
  $service = wsclient_service_load('dynect');
  
  $detail = $service->get_zone($zone);
  dpm($detail);
  return '';
}