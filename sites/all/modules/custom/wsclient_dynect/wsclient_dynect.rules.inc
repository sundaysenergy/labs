<?php

/**
 * Implements hook_rules_action_info()
 */
function wsclient_dynect_rules_action_info() {
  return array(
    'wsclient_dynect_add_zone' => array(
      'label' => t('Add DynECT Zone'),
      'parameter' => array(
        'domain' => array('type' => 'domains_domain', 'label' => t('Domain')),
      ),
      'group' => t('Domain'),
      'base' => 'wsclient_dynect_add_zone',
    ),
  );
}

/**
 * Action callback: add a zone
 */
function wsclient_dynect_add_zone($domain) {
  // reload entity
  $domain_entity = domains_domain_load($domain->id);
  $owner = user_load($domain->uid);
  
  $service = wsclient_service_load('dynect');
  
  $result = $service->create_zone($domain_entity->name, $owner->mail, $domain_entity->name, 3600);
  if($result['status'] == 'success') {
    // Publish zone
    $service->publish_zone($domain_entity->name);
    watchdog('dynect', 'Zone create successfull for @name', array('@name' => $domain_entity->name));
  }
  else {
    foreach($result['msgs'] as $message){
      $message = (object) $message;
      watchdog('dynect', $message->LVL.": ".($message->ERR_CD != '' ? '('.$message->ERR_CD.') ' : '').$message->SOURCE." - ".$message->INFO, array(), WATCHDOG_ERROR);
    }
  }
}
