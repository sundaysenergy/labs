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
  
  $detail = $service->get_all_record($zone);
  $records = array();
  if($detail['status'] == 'success') {
    foreach($detail['data'] as $record) {
      $record = str_replace('/REST/', '', $record);
      $tmp = explode('/', $record);
      $record_type = $tmp[0];
      array_shift($tmp);
      $record_detail = $service->get_record($record_type, implode('/', $tmp));
      if($record_detail['status'] == 'success') {
        $data = $record_detail['data'];
        $list = $data['rdata'];
        if($data['record_type'] == 'SOA') {
          $list = array();
          foreach($data['rdata'] as $key => $value) {
            $list[] = $key . ': ' . $value;
          }
        }
        $records[] = array(
          $data['fqdn'],
          $data['record_type'],
          $data['ttl'],
          theme('item_list', array('items' => $list)),
        );
      }
    }
  }
  $header = array(
    t('Name'), t('Type'), t('TTL'), t('Data')
  );
  return theme('table', array('rows' => $records, 'header' => $header));
}