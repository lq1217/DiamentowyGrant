// OpenAIL - Open Android Indoor Localization
// Copyright (C) 2015 Michal Nowicki (michal.nowicki@put.poznan.pl)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

package org.dg.wifi;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

import org.dg.graphManager.WiFiMeasurement;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

public class WiFiScanner extends BroadcastReceiver {
	
	final String moduleLogName = "WiFiScanner";

	// Class to access physical sensor
	WifiManager wifiManager;
	
	// Operation mode - single/continuous
	boolean continuousScanning = false, singleScan = true;
	
	// Are we waiting for the scan?
	public boolean waitingForScan = false;
	
	// Was start called?
	public boolean playbackActive = false;
	
	// Timestamps
	long startTimestampOfCurrentScan, startTimestampOfGlobalTime;

	// Scan id and graphId
	private int id, graphPoseId;

	// Class used to process wifi scans
	WiFiPlaceRecognition placeRecognition;

	// Last results
	List<MyScanResult> previousWiFiList = new ArrayList<MyScanResult>();
	
	

	// / G2O
	// Position index
	int posIndex = 10000;

	// Variable indicating new measurement
	boolean newMeasurement = false;

	
	// File to save data
	PrintStream outStreamRawData = null;
	
	// Should we save raw WiFi data
	boolean saveRawData = false;

		
	
	public WiFiScanner(
			WifiManager _wifiManager,
			org.dg.openAIL.ConfigurationReader.Parameters.WiFiPlaceRecognition wifiPlaceRecognitionParameters) {
		// Save physical access to sensor
		wifiManager = _wifiManager;
		
		// We operate in continuous mode
		continuousScanning = true;
		singleScan = false;
		
		// Let's assume 0's
		id = 0;
		graphPoseId = 0;
		startTimestampOfGlobalTime = 0;

		// Creating wifi place recognition
		placeRecognition = new WiFiPlaceRecognition(
				wifiPlaceRecognitionParameters);

		// Directory to story results
		File folder = new File(Environment.getExternalStorageDirectory()
				+ "/OpenAIL/WiFi");

		if (!folder.exists()) {
			folder.mkdir();
		}

		// File to save results
		saveRawData = wifiPlaceRecognitionParameters.recordRawData;
		
	}

	// Nicely setting parameters
	public WiFiScanner singleScan(boolean _singleScan) {
		this.singleScan = _singleScan;
		return this;
	}

	public WiFiScanner continuousScanning(boolean _continuousScanning) {
		this.continuousScanning = _continuousScanning;
		return this;
	}

//	public WifiScanner startTimestampOfGlobalTime(
//			long _startTimestampOfGlobalTime) {
//		this.startTimestampOfGlobalTime = _startTimestampOfGlobalTime ;
//		return this;
//	}

	// We start the recognition of places
	public void startNewPlaceRecognitionThread() {
		placeRecognition.startRecognition();
	}

	// We stop the recognition of places
	public void stopNewPlaceRecognitionThread() {
		placeRecognition.stopRecognition();
	}

	// Are we waiting for the WiFi scan?
	public boolean getWaitingForScan() {
		return waitingForScan;
	}
	
	public boolean getPlaybackState() {
		return playbackActive;
	}
	
	public void setPlaybackState(boolean playbackState) {
		playbackActive = playbackState;
	}

	// Starts scanning
	public void startScanning() {
				
		if (continuousScanning && saveRawData) {

			String fileName = "";
			fileName = Environment.getExternalStorageDirectory().toString()
					+ "/OpenAIL/rawData/wifi.log";

			// RawMeasurements
			FileOutputStream foutStream;
			try {
				foutStream = new FileOutputStream(fileName);
				outStreamRawData = new PrintStream(foutStream);
			} catch (FileNotFoundException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}

		}
		
		startTimestampOfCurrentScan = System.nanoTime();

		// We initialize the WiFi scan if WiFi is on
		Log.e("WIFI",
				"WIFI START STATE: " + wifiManager.isWifiEnabled() + " "
						+ wifiManager.pingSupplicant() + " "
						+ wifiManager.getWifiState());
		if (wifiManager.isWifiEnabled() && (singleScan || continuousScanning)) // &&
																				// !getRunningState())
		{
			wifiManager.startScan();
			waitingForScan = true;
		}
	}

	// Stops scanning - we still wait for started scan
	public void stopScanning() {
		continuousScanning = false;

//		placeRecognition.closeSaveStream();

		// Close cleanly
		if (outStreamRawData != null) {
			outStreamRawData.close();
			outStreamRawData = null;
		}
	}

	
	
	// Info for GUI
	public int getNetworkCount() {
		return previousWiFiList.size();
	}

	// Info for GUI
	public String getStrongestNetwork() {
		String bestName = "";
		int bestLvl = -150;

		for (int i = 0; i < previousWiFiList.size(); i++) {
			MyScanResult scanResult = previousWiFiList.get(i);
			if (scanResult.level > bestLvl) {
				bestLvl = scanResult.level;
				bestName = scanResult.networkName;
			}
		}
		return bestName;
	}
	
	// Return the size of place database
	public int getSizeOfPlaceDatabase() {
		return placeRecognition.getSizeOfPlaceDatabase();
	}

	// Returns the last scan
	public List<MyScanResult> getLastScan()
	{
		List<MyScanResult> myList = new ArrayList<MyScanResult>();
		synchronized (previousWiFiList) {
			for (MyScanResult sr : previousWiFiList) {
				myList.add(new MyScanResult(sr.BSSID, sr.level, sr.networkName, sr.frequency));
			}
		}
		
		return myList;
	}
	
	// Adds last scan to place recognition
	public void addLastScanToRecognition(int id) {
		graphPoseId = id; 
		synchronized (previousWiFiList) {
			addScanToRecognition(id, previousWiFiList);
		}
	}

	// Adding scan to place rec
	public void addScanToRecognition(int id, List<MyScanResult> wifiScan) {
		if (wifiScan.size() > 0)
			placeRecognition.addPlace(wifiScan, id);
	}

	// Returns the places that we recognized and clears the list
	public List<org.dg.openAIL.IdPair<Integer, Integer>> getAndClearRecognizedPlacesList() {
		return placeRecognition.getAndClearRecognizedPlacesList();
	}

	
	

	

	// Are there new Wifi scans?
	public boolean isNewMeasurement() {
		return newMeasurement;
	}

	// we set the value of new measurement
	public void newMeasurement(boolean value) {
		newMeasurement = value;
	}

	// Setting the start of the time
	public void setStartTime() {
		Log.d(moduleLogName, "setStartTime = " + System.nanoTime() /10e9);
		startTimestampOfGlobalTime = System.nanoTime();
	}

	// When data from physical sensors is received
	@Override
	public void onReceive(Context arg0, Intent arg1) {
		Log.d(moduleLogName, "Scan finished\n");
		if (waitingForScan) {
			try {
				// Process list of detected WiFis
				List<ScanResult> tmp = wifiManager.getScanResults();
				Log.d(moduleLogName, "Found " + tmp.size() + " wifis \n");
				
				List<MyScanResult> wifiList = new ArrayList<MyScanResult>();
				for (ScanResult network : tmp) {
					wifiList.add(new MyScanResult(network.BSSID, network.level, network.SSID, network.frequency));
				}
				
				processNewScan(wifiList);
				
				
				Toast toast = Toast.makeText(arg0.getApplicationContext(),
						"WiFi scan finished", Toast.LENGTH_SHORT);
				toast.show();
			} catch (Exception e) {
				Log.d(moduleLogName, "Scanning failed: " + e.getMessage() + "\n");

				Toast toast = Toast.makeText(arg0.getApplicationContext(),
						"WiFi scan FAILED", Toast.LENGTH_SHORT);
				toast.show();
			}
			
			// Prepare for next measurement
			waitingForScan = false;
			if (continuousScanning) {
				startTimestampOfCurrentScan = System.nanoTime();
				boolean value = wifiManager.startScan();
				waitingForScan = true;
				Log.d(moduleLogName,
						"Called start, waiting on next scan - startScan value: "
								+ value + "\n");
			}
			id++;
		}
	}
	
	// Simulate working with provided scan
	public void playback(List<MyScanResult> wifiScans) {
		try {
			processNewScan(wifiScans);
			
			Log.d(moduleLogName, "WiFi scan finished");

		} catch (Exception e) {
			Log.e(moduleLogName, "WiFi scan FAILED");
		}
	}

	/**
	 * Processes new scan coming either from real sensor manager or from playback
	 */
	private void processNewScan(List<MyScanResult> wifiScan) throws InterruptedException {

			// Saving raw data
			if (saveRawData)
				saveScanToRawStream(wifiScan);
				
			// Save measurement
			synchronized (previousWiFiList) {
				previousWiFiList = wifiScan;
			}

			newMeasurement = true;	
	}

	/**
	 * @param wifiScans
	 */
	private void saveScanToRawStream(List<MyScanResult> wifiScans) {
		Log.d(moduleLogName, "current =  " + System.nanoTime()/10e9 + "  startTimestampOfCurrentScan = " + startTimestampOfCurrentScan/10e9 + "  startTimestampOfGlobalTime = " + startTimestampOfGlobalTime/10e9);
		outStreamRawData.print(Integer.toString(graphPoseId)
				+ "\t"
				+ Integer.toString(id)
				+ "\t"
				+ Long.toString(startTimestampOfCurrentScan
						- startTimestampOfGlobalTime)
				+ "\t"
				+ Long.toString(System.nanoTime()
						- startTimestampOfGlobalTime) + "\t");
		outStreamRawData.print(wifiScans.size() + "\n");

		// Save BSSID, SSID, lvl and frequency
		for (int i = 0; i < wifiScans.size(); i++) {
			MyScanResult scanResult = wifiScans.get(i); //convertLevelToMeters(scanResult.level)  + "\t" + scanResult.frequency 
			outStreamRawData.print(scanResult.BSSID + "\t"
					+ scanResult.networkName + "\t" + scanResult.level + "\t" + scanResult.frequency + "\n");
		}
	}


}
