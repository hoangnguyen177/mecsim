<?php

/**
 * @file
 * take care of creating new experiment
 */

/**
 * Display name of the experiment.
 * Create a folder for that experiment
 *
 * @return
 *   A form array
 * This form will use ajax to check whether any name like that exist in the database or not
 * Create a folder with the name, and add it into the database
 */
function new_experiment_form($form, &$form_state) {
	//get the id
	//later
	//
	$form['desc'] = array(
	    '#type' => 'item',
	    '#title' => 'New Experiment',
  	);

	//experiment name
	$form['name'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Experiment Name'),
	    '#weight' => '-2',
	);

	//experiment description
	$form['description'] = array(
	    '#type' => 'textarea',
	    '#title' => t('Experiment Description'),
		'#weight' => '-1',
	);	

	// Adds a simple submit button that refreshes the form and clears its
	// contents. This is the default behavior for forms.
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Create',
	);
	return $form;

}


/**
* validation of the form
*/
function new_experiment_form_validate($form, &$form_state){
	global $user;
	$authed = false;
   	//roles refers to table roles and roles[rid]
  	if (isset($user->roles[3])) {
	    $authed = true;
	    drupal_set_message(t('You are an administrator user'), 'status');
	}elseif (isset($user->roles[2])){
	    drupal_set_message(t('You are an authenticated user'), 'status');
		$authed = true;
	}elseif (isset($user->roles[4])){
	    drupal_set_message(t('You are an editor user'), 'status');
		$authed = true;
	}else{
	    form_set_error('', t('You are not authenticated'));
  	}

  	if($authed == true){
  		if(empty($form_state['values']['name'])){
	  		form_set_error('', t('name cannot be null'));	  			
  		}
  	}

}

/**
* submit form
*/
function new_experiment_form_submit($form, &$form_state) {
	global $user;
	$exp_name = trim($form_state['values']['name']);
	$exp_description = trim($form_state['values']['description']);
	$username = $user->name;
	$creationtime = REQUEST_TIME;
	// they use staff_number in griffith 
	// not sure where they got that from ???? enter manually ????
	// we get username then
	// /var/www/workflows/custom/		
	$exp_location = $_SERVER['DOCUMENT_ROOT'].base_path()."sites/default/files/mecsim/$username/$creationtime";
	exec ('mkdir '.$exp_location);
	// not that exp_name cannot be empty
	if(!empty($exp_name) and !empty($username)){
		//insert into 'mecsim'
		$txn = db_transaction();
		try{
			$id = db_insert('mecsim')
		  		->fields(array(
		    		'name' => $exp_name,
		    		'description' => $exp_description,
		    		'creation_time' => $creationtime,
		    		'exp_location' => $exp_location,
		    		'owner' => $username,
		    		'status' => 0,
		  			)
				)->execute();
	  		$_SESSION['mecsim_experiment']= array(
      			'id' => $id,  
    		);
    		$form_state['redirect'] = 'mecsim/experiment';			
		}
		catch(Exception $e){
			$txn->rollback();
			watchdog_exception('type', $e);
			drupal_set_message(t('Error inserting into database. @new_experiment_form_submit'));
		}
	}
	else{
		drupal_set_message(t('Experiment name must not be empty'), 'error');
	}
}


