"use strict";
/**
 *	device.js
 */

/* class Device */
function Device(aConn,aGui,aDevicesManager)
{
	var arrFilesToRetrieve = [] ;
	var retrieveFileBusy=0;
	var currFile;
	var curr_dev = null;
	var arrCurrParamStatus = [];
	var cfg_arr = null;
	var enableDisplay = 0;
	var currStatusName;
	var fileTimeout=0;
	var currParamNum;
	var retrieveStatusBusy=0;
	var thisDevice=this;
	var getStatusTask=null;
	var FAST_REFRESH_RATE=500;
	var SLOW_REFRESH_RATE=5000;
	var currStatusRefreshRate;
	
	function parse_cfg_arr(err,text) 
	{
		cfg_arr = JSON.parse(text);
		if(1 == enableDisplay)
		{
			display();
		}
	}

	/*function hide */
	this.hide = function()
	{
		enableDisplay = 0;
		arrCurrParamStatus=[];
		curr_dev=null;
	}
	
	/*function show */
	this.show = function()
	{
		enableDisplay = 1;
		display();
	}

	/*function close */
	this.close = function()
	{
		clearInterval(fGetFileTask);
		clearTimeout(getStatusTask);
		clearTimeout(pollingTask);
		aConn.abort();
		aGui.clearDevice();		
	}

	function parse_cmd_set_replay(text)
	{
		//if("done" != text)
	}

	function fSelectChangeCallback(selCtrlName,selVal)
	{
		arrFilesToRetrieve.push({ url : "cmd/++set_dev_param/"+curr_dev.name+"/" + 
			selCtrlName + "/" +  selVal, callbackFunc : parse_cmd_set_replay });
	}
		
	/*func: display */
	function display()
	{
		aGui.clearDevice();
		
		if(null == cfg_arr) return ;

		aGui.setDefaultCtrlView();
		
		for( var i = 0; i < cfg_arr.length; i++ ) 
		{
			var dev = cfg_arr[i];
			aGui.addDev(dev.name,dev.type);
	    }		
	}
	
	/*function update_dev_pane */
	function update_dev_pane(dev) 
	{
		aGui.clearCtrlView()
		var dev_params=Object.keys(dev);
		var val;
		for ( var j = 0; j < dev_params.length; j++ )
		{
			var param=dev_params[j];
			if("controls"==param)
			{
				var ctrl_arr = dev[param];
				for ( var k = 0; k < ctrl_arr.length; k++ )
				{
					var ctrl = ctrl_arr[k];
					if("select_switch" == ctrl.type)
					{
						aGui.addDevParam("select",ctrl.name,
								{selCallback : fSelectChangeCallback , values : ctrl.values});
						arrCurrParamStatus.push(ctrl.name);
					}
				}
			}
			else
			{
				aGui.addDevParam("info",param,dev[param])
			}
		}
	}
	
	
	function changeCurrDevCallback(newDevName) 
	{
		clearTimeout(getStatusTask);
		for ( var i = 0; i < cfg_arr.length; i++ ) 
		{
			var dev=cfg_arr[i];
			if(newDevName == dev.name)
			{
				curr_dev=dev;
				arrCurrParamStatus = [];
				update_dev_pane(dev);					
				currParamNum=0;
				retrieveStatusBusy = 0;
				if(arrCurrParamStatus.length)
				{
					currStatusRefreshRate=FAST_REFRESH_RATE/arrCurrParamStatus.length;
					getStatusTask=setTimeout(fGetStatusTask, currStatusRefreshRate);
				}
				break;
			}
		}
		
	}

	/*function parse_status */
	function parse_status(err,text)
	{
		if(0!=err) text="undefined";
		
		if(0 != retrieveStatusBusy)
		{
		    var lines = text.split(/[\n\r\0]+/);		
		    for(var i=0 ; i<lines.length ; i++)
		    {
		    	if("" != lines[i])
		    	{
		    		aGui.updateParamStatus(currStatusName , lines[i]);
		    		break;
		    	}
		    }

			currParamNum++ ;
			if (currParamNum >= arrCurrParamStatus.length)
			{
				currStatusRefreshRate=SLOW_REFRESH_RATE/arrCurrParamStatus.length;
				currParamNum=0;
			}
			getStatusTask=setTimeout(fGetStatusTask, currStatusRefreshRate);
		}
	}

	/*function statusPolling */
	var fGetStatusTask = function()
	{
		retrieveStatusBusy=1;
		currStatusName = arrCurrParamStatus[currParamNum];
		arrFilesToRetrieve.push({ url : "cmd/++get_dev_param/"+curr_dev.name+"/"+currStatusName , callbackFunc : parse_status });		
	}

	function fRcvCallback(err,aData)
	{
		if(1 == retrieveFileBusy)
		{
			if((0==err)&&(null!=aDevicesManager))
			{
				aDevicesManager.setDeviceAlive(thisDevice);
			}

			if(null != currFile.callbackFunc)
			{
				currFile.callbackFunc(err,aData);
			}
			
			retrieveFileBusy = 0 ;
		}
	}
	
	/*function for fGetFileTask */
	var fGetFileTask= function()
	{
		if(0 == retrieveFileBusy)
		{
			fileTimeout=0;
			if ( 0 < arrFilesToRetrieve.length)
			{
				retrieveFileBusy = 1;
				currFile = arrFilesToRetrieve[0];
				arrFilesToRetrieve.shift();
				printf.dbg("device send "+ currFile.url);
				aConn.send(currFile.url);
			}
		}
		else
		{
			fileTimeout++;
			if(25==fileTimeout)
			{
				aConn.replyNotRecievedYet();
			}
			else if(50==fileTimeout)
			{
				printf.dbg("conn timeout\r\n");
				aConn.abort();
				parse_status(1,"");
				retrieveFileBusy = 0 ;
				retrieveStatusBusy=0;
			}
		}
	}
	

	function parse_polling_reply(err,text) 
	{
		setTimeout(fPollingTask, 5000);
	}
	
	/*function for fPollingTask */
	var fPollingTask= function()
	{
		arrFilesToRetrieve.push({ url : "cmd/++get_serial" , callbackFunc : parse_polling_reply });		
	}
	
	aGui.setDevChangeCallback(changeCurrDevCallback);
	aConn.setReceiveCallback(fRcvCallback);
	
	arrFilesToRetrieve.push({ url : "board.cfg" , callbackFunc : parse_cfg_arr });


	var getFilesTask = setInterval(fGetFileTask, 200);
	var pollingTask = setTimeout(fPollingTask, 5000);

}
