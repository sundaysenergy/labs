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
    $service->publish_zone($values['name']);
    drupal_set_message('Zone create success, please publish it');
  }
  else {
    foreach($result['msgs'] as $message){
      $message = (object) $message;
      drupal_set_message($message->LVL.": ".($message->ERR_CD != '' ? '('.$message->ERR_CD.') ' : '').$message->SOURCE." - ".$message->INFO, 'error');
    }
  }
}

function wsclient_dynect_create_record_form($form, $form_state) {
  $form['fqdn'] = array(
    '#type' => 'textfield',
    '#title' => t('Record name'),
    '#description' => t('Only input sub record you need to add, like www or server1'),
  );
  $form['zone'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain name'),
    '#required' => TRUE,
  );
  $record_type = _wsclient_dynect_support_type();
  $form['record_type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => $record_type,
    '#required' => TRUE,
  );
  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add new record'),
  );
  return $form;
}

function wsclient_dynect_create_record_form_validate($form, $form_state) {
  $values = $form_state['values'];
  if(!domains_validate_name($values['zone'])) {
    form_set_error('zone', t('Please enter a valid domain like example.com'));
  }
}

function wsclient_dynect_create_record_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $service = wsclient_service_load('dynect');
  
  // TODO: Write method to fill data here, for A and AAA record we are using address, CName we will using cname
  $rdata = array('address' => $values['value']);
  // Current only A record support
  
  $result = $service->create_record($values['record_type'], $values['zone'], !empty($values['fqdn']) ? $values['fqdn'] . '.' . $values['zone'] : $values['zone'], $rdata);
  if(is_object($result)) $result = json_decode ($result->body);
  $result = (array) $result;
  if($result['status'] == 'success') {
    foreach($result['msgs'] as $message){
      $message = (object) $message;
      drupal_set_message($message->LVL.": ".($message->ERR_CD != '' ? '('.$message->ERR_CD.') ' : '').$message->SOURCE." - ".$message->INFO);
    }
    $service->publish_zone($values['zone']);
    drupal_goto('dynect/zone/' . $values['zone']);
  }
  else {
    foreach($result['msgs'] as $message){
      $message = (object) $message;
      drupal_set_message($message->LVL.": ".($message->ERR_CD != '' ? '('.$message->ERR_CD.') ' : '').$message->SOURCE." - ".$message->INFO, 'error');
    }
    $form_state['rebuild'] = TRUE;
  }
}