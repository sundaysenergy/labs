<?php

/**
 * @file
 */

/**
 * Get records for a domain
 */
function domains_dns_get_records($domain) {
  try {
    return domains_dns_get_provider($domain)->getRecords($domain);
  }
  catch (Exception $ex) {
    _domains_handle_exception($ex);
  }
}

/**
 * Get supported record types by provider
 * @return array
 */
function domains_dns_get_record_types($provider) {
  if (is_object($provider)) {
    $provider = $provider->dns_provider;
  }

  return domains_dns_get_provider($provider)->getRecordTypes();
}

/**
 * Get domain provider object
 * @param string $name
 * @return DomainDnsProvider
 */
function domains_dns_get_provider($name) {
  $objects =& drupal_static(__FUNCTION__);

  if (is_object($name)) $name = $name->dns_provider;

  // lazy constructor
  if (empty($objects[$name])) {
    $info = domains_dns_get_providers($name);

    if ($info) {
      $class = $info['class'];
      $objects[$name] = new $class($info);
    }
    else {
      return FALSE;
    }
  }

  return $objects[$name];
}

/**
 * Get domain controller info
 * @return array
 */
function domains_dns_get_providers($name=NULL) {
  $controllers =& drupal_static(__FUNCTION__);

  if ($controllers === NULL) {
    $controllers = array();

    foreach (module_implements('domains_dns_provider_info') as $module) {
      $hook_function = "{$module}_domains_dns_provider_info";

      $controllers += $hook_function();
    }
  }

  if ($name) {
    return isset($controllers[$name]) ? $controllers[$name] : NULL;
  }
  else {
    return $controllers;
  }
}

/**
 * Implement hook_domains_dns_provider_info()
 */
function domains_domains_dns_provider_info() {
  return array(
    'custom' => array(
      'label' => t('Custom'),
      'class' => 'CustomDnsProvider',
      'configure form' => 'domains_custom_dns_configure_form',
      'record management' => FALSE,
    ),
  );
}

/**
 * Form call back for custom DNS configuration form
 */
function domains_custom_dns_configure_form(&$form, &$form_state) {
  $domain = $form['#domain'];

  $ns = domains_get_namesevers($domain);
  for ($i = 0; $i < 6; $i++) {
    $form['settings']['ns'][$i] = array(
      '#type' => 'textfield',
      '#title' => t('Namesever @i', array('@i' => $i + 1)),
      '#default_value' => isset($ns[$i]) ? $ns[$i] : '',
      '#required' => $i < 2, // At least have two available namesevers.
    );
  }  
}
