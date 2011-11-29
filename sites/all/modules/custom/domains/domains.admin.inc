<?php

/**
 * Landing configuration page
 */
function domains_admin_config_page() {
  // nothing now
}

/**
 * Callback for page /admin/config/domains/manage.
 */
function domain_info_page_callback() {
  return t('Welcome to the administration page for domains!');
}

/**
 * Provider admin page
 */
function domains_admin_providers() {
  $header = array(t('Provider'), t('Machine name'), array('data' => t('Operations')));
  $rows = array();

  $controllers = domains_get_providers();

  if (count($controllers)) {
    foreach ($controllers as $name => $info) {
      $row = array();
      
      $row[] = $info['label'];
      $row[] = $name;
      $row[] = l(t('configure'), DOMAINS_ADMIN_URL . '/providers/' . $name . '/config');
      
      $rows[] = $row;
    }
  }
  else {
    $rows[][] = array(
      'data' => t('There is no available domain providers.'),
      'colspan' => 3,
    );
  }
  
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
}

/**
 * Domain provider configuration page
 */
function domains_admin_provider_config($name) {
  $provider = domains_get_provider($name);

  if (!$provider) {
    return drupal_not_found();
  }

  // TODO: May be we should use a "config form" key instead ?
  return drupal_get_form('domains_admin_provider_config_form', $provider);
}

function domains_admin_provider_config_form($form, &$form_state, $provider) { 
  $info = $provider->getInfo();
  $ui = $provider->getUiController();

  $form['label'] = array(
    '#type' => 'item',
    '#title' => t('Provider'),
    '#markup' => $info['label'],
  );

  $form['#provider'] = $provider;
  $form['#ui_controller'] = $ui;

  if ($ui != null) {
    try {
      $ui->buildConfigForm($form, $form_state);
    }
    catch (Exception $ex) {
      _domains_handle_exception($ex);
    }
  }
  else {
    $form['markup']['#markup'] = t('There is no configuration option for this provider.');
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['save'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

function domains_admin_provider_config_form_validate($form, &$form_state) {
  if ($form['#ui_controller']) {
    $form['#ui_controller']->validateConfigForm($form, $form_state);
  }
}

function domains_admin_provider_config_form_submit($form, &$form_state) {
  if ($form['#ui_controller']) {
    try {
      $form['#ui_controller']->saveConfigForm($form, $form_state);
      drupal_set_message(t('Your configuration has been saved.'));
    }
    catch (Exception $ex) {
      _domains_handle_exception($ex);
    }
  }
}