<?php

/**
 * Domain dns form
 */
function domains_domain_dns_form($form, &$form_state, $domain) {
  $providers = domains_dns_get_providers();

  $form['#domain'] = $domain;
  $form['#providers'] = $providers;
  
  $form['provider'] = array(
    '#title' => t('DNS Provider'),
    '#description' => t('Please select a DNS provider for this domain.'),
    '#type' => 'select',
    '#options' => array(
      'enom' => t('Enom'),
      'dynect' => t('DynECT'),
      'custom' => t('Custom'),
    ),
    '#default_value' => $domain->dns_provider,
    '#ajax' => array(
      'callback' => 'domains_domain_dns_form_ajax_submit',
      'wrapper' => 'settings-div',
    ),
  );

  $dns_provider = $domain->dns_provider;
  if (isset($form_state['values'])) {
    $dns_provider = $form_state['values']['provider'];
  }

  $provider_info = $providers[$dns_provider];

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
    '#prefix' => '<div id="settings-div">',
    '#suffix' => '</div>',
  );
  
  if (isset($provider_info['configure form'])) {
    $provider_info['configure form']($form, $form_state);
  }
  else {
    $form['settings']['markup'] = array(
      '#markup' => t('This provider has no configuration option.'),
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function domains_domain_dns_form_ajax_submit($form, &$form_state) {
  return $form['settings'];
}

/**
 * DNS nameserver form callback
 */
function domains_domain_dns_form_submit($form, &$form_state) {
  $domain = $form['#domain'];
  $new_provider = $form_state['values']['provider'];
  $success = TRUE;
  $ns = NULL;
  
  try {
    $settings = isset($form_state['values']['settings']) ? (array)$form_state['values']['settings'] : array();
    $ns = domains_dns_get_provider($new_provider)->activate($domain->name, $settings);

    if (is_array($ns)) {
      $success = domains_set_nameservers($domain, $ns);
    }
  } catch (Exception $ex) {
    _domains_handle_exception($ex);
    $success = FALSE;
  }

  if (TRUE === $success) {
    $domain->dns_provider = $new_provider;
    domains_domain_save($domain);
    drupal_set_message(t('Domain nameserver has been updated.'));
  }
}

/**
 * Domain dns records form
 */
function domains_domain_dns_records_form($form, &$form_state, $domain) {
  $info = domains_dns_get_providers($domain->dns_provider);
  if (empty($info['record management'])) {
    drupal_set_message(t('This DNS provider does not support record management.'), 'error');
    return $form;
  }

  //$records = domains_dns_get_records($domain);
  $form_state['more'] = isset($form_state['more']) ? $form_state['more'] : 10;
  $form['records'] = array(
    '#tree' => TRUE,
  );

  for ($i = 0; $i < $form_state['more'];$i++) {
    $form['records'][$i] = array(
      '#type' => 'textfield',
      '#title' => t('Record @i', array('@i' => $i+1)),
    );
  }
  
  $form['more'] = array(
    '#type' => 'submit',
    '#value' => t('More'),
    '#submit' => array('domains_domain_dns_records_more_submit'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  return $form;
}

/**
 * Submit callback for more button
 */
function domains_domain_dns_records_more_submit($form, &$form_state) {
  $form_state['more'] += 5;
  $form_state['rebuild'] = TRUE;
}

function domains_domain_dns_records_form_submit() {
  
}
