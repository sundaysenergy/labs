<?php

/**
 * @file
 * 
 * Rules integration for domains module
 */

/**
 * Implements hook_rules_event_info()
 */
function domains_rules_event_info() {
  return array(
    'domain_purchase' => array(
      'label' => t('After purchasing a domain'),
      'group' => t('Domain'),
      'variables' => array(
        'domain' => array('type' => 'domains_domain', 'label' => t('Target Domain')),
      ),
    ),
    'domain_renew' => array(
      'label' => t('After renewing a domain'),
      'group' => t('Domain'),
      'variables' => array(
        'domain' => array('type' => 'domains_domain', 'label' => t('Target Domain')),
      ),
    ),
  );
}

/**
 * Implements hook_rules_action_info()
 */
function domains_rules_action_info() {
  return array(
    'domain_renew' => array(
      'label' => t('Renew a domain'),
      'parameter' => array(
        'domain' => array('type' => 'domains_domain', 'label' => t('Domain')),
      ),
      'group' => t('Domain'),
      'base' => 'domains_action_domain_renew',
    ),
  );
}

/**
 * Action callback: renew a domain
 */
function domains_action_domain_renew($domain) {
  // reload entity
  $domain_entity = domains_domain_load($domain->id);
  domains_renew($domain_entity, 1);
}
