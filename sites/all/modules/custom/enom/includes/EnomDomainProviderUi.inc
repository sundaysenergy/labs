<?php

class EnomDomainProviderUi extends DomainProviderUI {
  public function buildConfigForm(&$form, &$form_state) {
    $form['login_info'] = array(
      '#type' => 'fieldset',
      '#title' => t('API information'),
      '#collapsible' => TRUE,
    );

    $form['login_info']['note'] = array(
      '#markup' => t('Enom API can be configured at Web Services interface.'),
    );

    try {
      $account_info = $this->provider->invoke('GetAllAccountInfo');
    }
    catch (Exception $e) {
      $account_info = array();
    }

    $form['#default_info'] = $account_info;

    $form['account'] = array(
      '#type' => 'fieldset',
      '#title' => t('Account information'),
      '#description' => t('This information is also used for default registration information for new domain registered by Enom.'),
      '#collapsible' => TRUE,
      '#tree' => TRUE,
    );

    $form['account']['login'] = array(
      '#type' => 'fieldset',
      '#title' => t('Login information'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['account']['login']['uid'] = array(
      '#type' => 'item',
      '#title' => t('User ID'),
      '#markup' => isset($account_info['UserID']) ? $account_info['UserID'] : ''
    );

    $form['account']['login']['password'] = array(
      '#type' => 'password_confirm',
      '#description' => '!!!'  .  t('To change the current password, enter the new password in both fields.'),
    );

    $form['account']['login']['question'] = array(
      '#type' => 'select',
      '#title' => t('Secret question'),
      '#options' => array(
        'sbirth' => t('City of birth'),
        'ssocial' => t('Last 4 digits of SSN'),
        'shigh' => t('Highschool'),
      ),
      '#required' => TRUE,
      '#default_value' => isset($account_info['AuthQuestionType']) ? $account_info['AuthQuestionType'] : '',
    );

    $form['account']['login']['answer'] = array(
      '#type' => 'textfield',
      '#title' => t('Secret word'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['AuthQuestionAnswer']) ? $account_info['AuthQuestionAnswer'] : '',
    );

    $form['account']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['EmailAddress'])? $account_info['EmailAddress'] : ''
    );

    $form['account']['organization'] = array(
      '#type' => 'textfield',
      '#title' => t('Organization name'),
      '#default_value' => isset($account_info['OrganizationName']) ? $account_info['OrganizationName'] : '',
    );

    $form['account']['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#description' => t('Required if Organization name is used.'),
      '#default_value' => isset($account_info['JobTitle']) ? $account_info['JobTitle'] : '',
    );

    $form['account']['firstname'] = array(
      '#type' => 'textfield',
      '#title' => t('First name'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['Fname']) ? $account_info['Fname'] : '',
    );

    $form['account']['lastname'] = array(
      '#type' => 'textfield',
      '#title' => t('Last name'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['Lname']) ? $account_info['Lname'] : '',
    );

    $form['account']['address1'] = array(
      '#type' => 'textfield',
      '#title' => t('Address 1'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['Address'][0]) ? $account_info['Address'][0] : '',
    );

    $form['account']['address2'] = array(
      '#type' => 'textfield',
      '#title' => t('Address 2'),
      '#default_value' => isset($account_info['Address'][1]) ? $account_info['Address'][1] : '',
    );

    $form['account']['phone'] = array(
      '#type' => 'textfield',
      '#title' => t('Phone'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['Phone']) ? $account_info['Phone'] : ''
    );

    $form['account']['fax'] = array(
      '#type' => 'textfield',
      '#title' => t('Fax'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['Fax']) ? $account_info['Fax'] : ''
    );

    $form['account']['country'] = array(
      '#type' => 'textfield',
      '#title' => t('Country'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['Country']) ? $account_info['Country'] : '',
    );

    $form['account']['state_provine'] = array(
      '#type' => 'textfield',
      '#title' => t('State / Provine'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['StateProvince']) ? $account_info['StateProvince'] : '',
    );

    $form['account']['city'] = array(
      '#type' => 'textfield',
      '#title' => t('City'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['City']) ? $account_info['City'] : '',
    );

    $form['account']['postal_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Postal Code'),
      '#required' => TRUE,
      '#default_value' => isset($account_info['PostalCode']) ? $account_info['PostalCode'] : '',
    );
  }

  public function validateConfigForm($form, &$form_state) {
    $account = $form_state['values']['account'];
    if ($account['organization'] != '') {
      // Require additional fields.
      if ($account['title'] == '') {
        form_set_error('account][title', t('Title is required when Organization name is used.'));
      }
      if ($account['fax'] == '') {
        form_set_error('account][fax', t('Fax is required when Organization name is used.'));
      }
    }

    // Validate password
    if ($account['login']['password'] != '') {
      if (strlen($account['login']['password']) < 6 || strlen($account['login']['password']) > 20) {
        form_set_error('account][login][password', t('Password length must be between 6 to 20 characters.'));
      }
      if (preg_match('#[\#\s%&]#', $account['login']['password'])) {
        form_set_error('account][login][password', t('Password can not contain space, #, % and &.'));
      }
    }
  }

  public function saveConfigForm($form, &$form_state) {
    $default_info = $form['#default_info'];

    $account = $form_state['values']['account'];

    // Set original password.
    if ($account['login']['password'] == '') {
      $account['login']['password'] = $default_info['Password'];
    }

    $account_info = array(
      'Account' => $default_info['Account'],
      'NewUID' => $default_info['UserID'],
      'RegistrantEmailAddress' => $account['email'],
      'NewPW' => $account['login']['password'],
      'ConfirmNewPW' => $account['login']['password'],
      'AuthQuestionType' => $account['login']['question'],
      'AuthQuestionAnswer' => $account['login']['answer'],
      'RegistrantOrganizationName' => $account['organization'],
      'RegistrantFirstName' => $account['firstname'],
      'RegistrantLastName' => $account['lastname'],
      'RegistrantJobTitle' => $account['title'],
      'RegistrantAddress1' => $account['address1'],
      'RegistrantAddress2' => $account['address2'],
      'RegistrantCity' => $account['city'],
      'RegistrantStateProvinceChoice' => 'S',
      'RegistrantStateProvince' => $account['state_provine'],
      'RegistrantCountry' => $account['country'],
      'RegistrantPhone' => $account['phone'],
      'RegistrantFax' => $account['fax'],
      'RegistrantPostalCode' => $account['postal_code'],
    );

    $this->provider->invoke("UpdateAccountInfo", $account_info);
  }
}
