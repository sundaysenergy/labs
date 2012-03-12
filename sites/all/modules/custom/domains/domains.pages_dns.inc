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
  if ($new_provider != 'custom') {
    $updates = domains_dns_get_records($domain);
    domains_dns_update_records($new_provider, $updates);

    if (domains_dns_update_records($new_provider, $updates)) {
      try {
        $settings = isset($form_state['values']['settings']) ? (array)$form_state['values']['settings'] : array();
        $ns = domains_dns_get_provider($new_provider)->activate($domain->name, $settings);

        if (is_array($ns)) {
          $success = domains_set_nameservers($domain, $ns);
        }
      } 
			catch (Exception $ex) {
        _domains_handle_exception($ex);
        $success = FALSE;
      }

      if (TRUE === $success) {
        $domain->dns_provider = $new_provider;
        domains_domain_save($domain);
        drupal_set_message(t('Domain nameserver has been updated.'));
      } 
			else {
        drupal_set_message(t('Domain nameserver has not been updated.'));
	    }
    }
  }
}

/**
 * Domain dns records form
 */
function domains_domain_dns_records_form($form, &$form_state, $domain) {
  $info = domains_dns_get_providers($domain->dns_provider);
  $types = array('' => t('--')) + domains_dns_get_record_types($domain->dns_provider);

  if (empty($info['record management'])) {
    drupal_set_message(t('This DNS provider does not support record management.'), 'error');
    return $form;
  }

  if (empty($form_state['records'])) {
    $records = domains_dns_get_records($domain);
    usort($records, '_domains_dns_sort_record');
  }
  else {
    $records = $form_state['records'];
  }

  $form['#domain'] = $domain;

  $form['records'] = array(
    '#tree' => TRUE,
    '#theme' => 'domains_dns_records_form',
  );

  $form['#attached'] = array(
    'js' => array(
      drupal_get_path('module', 'domains') . '/js/dns_form.js',
    ),
  );

  $i = 0;
  foreach ($records as $record) {
    $form['records'][$i++] = domains_domain_dns_record_row($record, $types);
  }

  // Empty records.
  $form_state['more'] = isset($form_state['more']) ? $form_state['more'] : 5;
  for ($j = 0; $j < $form_state['more'];$j++, $i++) {
    $form['records'][$i] = domains_domain_dns_record_row(NULL, $types);
  }

  $form['more'] = array(
    '#type' => 'submit',
    '#value' => t('More'),
    '#submit' => array('domains_domain_dns_records_more_submit'),
    '#prefix' => '<div class="clearfix">',
    '#suffix' => '</div>',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

/**
 * Submit callback for more button
 */
function domains_domain_dns_records_more_submit($form, &$form_state) {
  $form_state['more'] += 5;
  $form_state['rebuild'] = TRUE;
}

/**
 * DNS form validation callback
 */
function domains_domain_dns_records_form_validate($form, &$form_state) {
  foreach ($form_state['values']['records'] as $element_index => $record) {
    switch ($record['type']) {
      case 'MX':
      case 'A':
      case 'CNAME':
        // TODO: Validate record value.
        if (empty($record['value'])) {
          form_set_error("records][$element_index][value", t('Record value is missing.'));
        }
        break;
    }
  }
}

function domains_domain_dns_records_form_submit($form, &$form_state) {
  $domain = $form['#domain'];

  // Array to track records state.
  $updates = array(
    'new' => array(),
    'delete' => array(),
    'change' => array(),
  );

  foreach ($form_state['values']['records'] as $element_index => $record) {
    $record['#record'] = $form['records'][$element_index]['#record'];

    if ($record['#record']) {
      if ($record['delete']) {
        $updates['delete'][] = $record;
      }
      else {
        // Check if the record is changed or not.
        foreach (array('name', 'type', 'value', 'priority') as $property) {
          if ($record[$property] != $record['#record'][$property]) {
            $updates['change'][] = $record;
            break;
          }
        }
      }
    }
    else {
      if ($record['type']) {
        $updates['new'][] = $record;
      }
    }
  }

  if (domains_dns_update_records($domain, $updates)) {
    drupal_set_message('DNS Records have been updated.');
  }
}

function domains_domain_dns_record_row($record, $types) {
  $row['#record'] = $record;

  if ($record && !empty($record['readonly'])) {
    $row['value'] = array(
      '#type' => 'item',
      '#markup' => isset($record['display']) ? $record['display'] : NULL,
    );
    $row['name'] = array(
      '#type' => 'item',
      '#markup' => isset($record['name']) ? $record['name'] : NULL,
    );
    $row['type'] = array(
      '#type' => 'item',
      '#markup' => $record['type'],
    );
    $row['priority'] = array(
      '#type' => 'item',
      '#markup' => isset($record['priority']) ? $record['priority'] : NULL,
    );
  }
  else {
    $row['name'] = array(
      '#type' => 'textfield',
      '#size' => 40,
      '#default_value' => isset($record['name']) ? $record['name'] : NULL,
    );
    $row['type'] = array(
      '#type' => 'select',
      '#options' => $types,
      '#default_value' => isset($record['type']) ? $record['type'] : NULL,
    );
    $row['value'] = array(
      '#type' => 'textfield',
      '#size' => 40,
      '#default_value' => isset($record['value']) ? $record['value'] : NULL,
    );
    $row['priority'] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#default_value' => isset($record['priority']) ? $record['priority'] : NULL,
    );
    if ($record) {
      $row['delete'] = array(
        '#type' => 'checkbox',
        '#default_value' => 0,
      );
    }
  }

  return $row;
}

function _domains_dns_sort_record($a, $b) {
  if (!empty($a['readonly']) || !empty($b['readonly'])) {
    if (empty($b['readonly'])) {
      return -1;
    }
    elseif (empty($a['readonly'])) {
      return 1;
    }
  }

  return strcmp($a['type'], $b['type']);
}