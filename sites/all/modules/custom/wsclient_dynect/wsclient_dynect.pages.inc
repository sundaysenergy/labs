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
        if(in_array($data['record_type'], array('SOA', 'SRV'))) {
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


// Create new zone
function wsclient_dynect_create_zone_form($form, $form_state) {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain name'),
    '#required' => TRUE,
  );
  $form['admin'] = array(
    '#type' => 'textfield',
    '#title' => t('Admin email address'),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add new zone'),
  );
  return $form;
}

function wsclient_dynect_create_zone_form_validate($form, $form_state) {
  $values = $form_state['values'];
  if(!domains_validate_name($values['name'])) {
    form_set_error('name', t('Please enter a valid domain like example.com'));
  }
  if(!valid_email_address($values['admin'])) {
    form_set_error('admin', t('Please enter a valid email address like admin@example.com'));
  }
}

function wsclient_dynect_create_zone_form_submit($form, $form_state) {
  $values = $form_state['values'];
  $service = wsclient_service_load('dynect');
  
  $result = $service->create_zone($values['name'], $values['admin'], $values['name'], 3600);
  if($result['status'] == 'success') {
    // Publish zone
    drupal_set_message('Zone create success, please publish it');
  }
  else {
    foreach($result['msgs'] as $message){
      $message = (object) $message;
      drupal_set_message($message->LVL.": ".($message->ERR_CD != '' ? '('.$message->ERR_CD.') ' : '').$message->SOURCE." - ".$message->INFO, 'error');
    }
  }
}