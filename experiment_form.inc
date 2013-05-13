<?php

/**
 * @file generates the form for one experiment
 */



/**
* overview page of the experiment
*/
function experiment_general_form($form, &$form_state){
	//get the id
	$expNum = $_SESSION['mecsim_experiment']['id'];
	// get the name and description of the experiment
	$result = db_query('SELECT m.name, m.description, m.nimrodid FROM {mecsim} m 
						WHERE m.exp_number=:expnum', array(':expnum'=> $expNum));
	//only one record anyway
	foreach ($result as $item) {
		$expName = $item->name;
		$expDescription = $item->description;
		$nimrodid = $item->nimrodid;
	}
	//header
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('Experiment Overview'),
  	);


	$form['overview'] = array(
    	'#type' => 'fieldset',
    	'#title' => t(''),
  	);

	//name
	$form['overview']['general_name'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Experiment Name:'),
	    '#default_value' => $expName,
	);

	//description
	$form['overview']['general_description'] = array(
	    '#type' => 'textarea',
	    '#title' => t('Description:'),
	    '#default_value' => $expDescription,
	);
	$form['overview']['submit'] = array(
	    '#type' => 'submit',	
	    '#value' => 'Save',
	    '#name' => 'save', 
	);
	//may be hidden type of nimrod id so that it can be performed when run is hit
	$form['nimrodid'] = array(
	    '#type' => 'hidden',
	    '#value' => $nimrodid,
	);
  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Run',
   	    '#name' => 'run', 
	);
	return $form;
}


/**
* submit from experiment_general_form
*/
function experiment_general_form_submit($form, &$form_state){
	$expNum = $_SESSION['mecsim_experiment']['id'];
	$button_name = $form_state['clicked_button']['#name'];
	if($button_name === 'save'){ //save the general form
		$newName = $form_state['values']['general_name'];
		$newDesc = $form_state['values']['general_description'];
		if(!empty($newName)){
			//insert into 'mecsim'
			$txn = db_transaction();
			try{
				$id = db_update('mecsim')
			  		->fields(array(
		    		'name' => $newName,
		    		'description' => $newDesc,
		  			)
				)
			  	->condition('exp_number', $expNum)
			  	->execute();
				drupal_set_message(t('Updated!'));
			}
			catch(Exception $e){
				$txn->rollback();
				watchdog_exception('type', $e);
				drupal_set_message(t('Error saving into database. @experiment_form_submit'), 'error');
			}
		}
		else{
			drupal_set_message(t('Experiment name must not be empty'), 'error');
		}
	}
	elseif($button_name === 'run'){  // run the experiment
		drupal_set_message(t('Run!'));
	}
}




/**
* experiment setting
*/
function experiment_setting_form($form, &$form_state) {
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('Experiment Parameters Settings'),
  	);

	//temperature
	$form['exp_temperature'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Temperature(K)'),
	);

	//voltage range


	//scan speed
	$form['exp_scan_speed'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Scan Speed(V/s)'),
	);

	//Pre-eqm switch
	$form['exp_preeqm_switch'] = array(
	    '#type' => 'checkbox',
	    '#title' => t('Pre-eqm Switch'),
	);	

	// AC Sources

	//Working electrode geomery



  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Save',
	);
	return $form;
}



function simulation_setting_form_submit($form, &$form_state){
	drupal_set_message(t('Simulation Params Updated!'));
}



/**
* simulation setting
*/
function simulation_setting_form($form, &$form_state) {
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('Simulation Parameters Settings'),
  	);
	//number of points to simulate
	$form['sim_simulation_points'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Number Of Points to Simulate'),
	);

	//2x points in time across n cycles
	$form['sim_cycle_points'] = array(
    	'#type' => 'textfield',
	   	'#title' => t('2x points in time across n cycles'),
	);

	//Electrode kinetics

	//Species:

	//Uncompensated resistance (Ohms):
	$form['sim_resistance'] = array(
    	'#type' => 'textfield',
	   	'#title' => t('Uncompensated Resistance (Ohms)'),
	);

	//Reactions:

	//Background capacitance:



  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Save',
	);
	return $form;

}


function experiment_setting_form_submit($form, &$form_state){
	drupal_set_message(t('Experiment Params Updated!'));
}


/**
* Post-processing setting
*/
function processing_setting_form($form, &$form_state) {
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('Post-Processing Options'),
  	);

  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Save',
	);
	return $form;
}


function processing_setting_form_submit($form, &$form_state){
	drupal_set_message(t('Processing Updated!'));
}



/**
* optimization setting
*/
function optimization_setting_form($form, &$form_state) {
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('Optimization Options'),
  	);

  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Save',
	    '#name' => 'optimization',	
	);
	return $form;
}



function optimization_setting_form_submit($form, &$form_state){
	drupal_set_message(t('Optimization Updated!'));
}



/**
* file management setting
*/
function file_management_setting_form($form, &$form_state) {
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('File Management Options'),
  	);

  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Save',
	    '#name' => 'file',	
	);
	return $form;
}



function file_management_setting_form_submit($form, &$form_state){
	drupal_set_message(t('File management Updated!'));
}


/**
* choose resouces for this experiment
*/
function resource_management_setting_form($form, &$form_state) {
	$form['_header'] = array(
    	'#type' => 'item',
    	'#title' => t('Experiment Resources'),
  	);

  	// submit and go back to the experiment list
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'Save',
	    '#name' => 'resource'
	);
	return $form;
}



function resource_management_setting_form_submit($form, &$form_state){
	drupal_set_message(t('Resources Updated!'));
}



