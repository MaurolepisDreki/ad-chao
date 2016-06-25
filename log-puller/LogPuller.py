#!/usr/bin/python
# @warning Requiers Python3 and Paramiko
# @file LogPuller.py
# @breif Pulls logs from GE Dicap (9900) X-Ray appliences
# @author Nile Aagard (MaurolepisDreki) <InventerNA@gmail.com>
# @warning This code has been modified from it's origional version to ensure that the security of these systems does not become compromised.

import paramiko
import re
import os
import shutil

user = "maintainance"
keyfile = user + ".key"
address = {
	"xray": ["10.0.0.1"],
	"sage": "logs.upload.example.com"
}
path = {
	"shell": "/tmp",
	"local": "./tmp",
	"remote": "/incoming/9900"
}

log_re = re.compile(r".+?\.tar\.gz")

class AllowAll(paramiko.client.MissingHostKeyPolicy):
	def missing_host_key(self, client, hostname, key):
		pass

def pull_logs():
	for remote in address["xray"]:
		# Open an SSH connection to the x-ray machine
		print("Connecting to {}... ".format(remote), end="", flush=True)
		ssh_inst = paramiko.client.SSHClient()
		ssh_inst.set_missing_host_key_policy(AllowAll())
		ssh_inst.connect(remote, username=user, key_filename=keyfile)
		print("SSH Credentials Accepted.")
	
		# Operate on an FTP-like protocol
		print("Negotiating FTP... ", end="", flush=True)
		sftp_inst = ssh_inst.open_sftp()
		sftp_inst.chdir(path["shell"])
		print("Done")
	
		# Get file list from the x-ray machine
		print("Requesting file list at {}... ".format(path["shell"]), end="", flush=True)
		file_list = sftp_inst.listdir()
		print(len(file_list))

		print("Filtering list for known log naming conventions... ", end="", flush=True)
		log_list = []
		for item in file_list:
			fname = log_re.match(item)
			if fname != None:
				log_list.append(item)
		del file_list
		print(len(log_list))

		# Copy Log files to local machine
		print("Checking local directory exists... ", end="", flush=True)
		if os.path.isdir(path["local"]):
			print("True")
		else:
			os.mkdir(path["local"])
			print("Created")

		for item in log_list:
			print("Pulling log {}... ".format(item), end="", flush=True)
			sftp_inst.get("{}/{}".format(path["shell"], item), "{}/{}".format(path["local"], item))
			print("Done")
	
		print("Closing connections... ", end="", flush=True)
		sftp_inst.close()
		ssh_inst.close()
		print("Done")

def push_logs():
	# Checking push path
	print("Checking remote's path... ", end="", flush=True)
	if os.path.isdir("//{}/{}".format(address["sage"], path["remote"])):
		print("Good")
	else:
		print("Fail")
		return
	
	# Check local path
	print("Checking local path... ", end="", flush=True)
	if os.path.isdir(path["local"]):
		print("Good")
	else:
		print("Fail")
		print("ERROR: ID-10T: Try pulling logs *before* pushing them!")
		return
	
	# Get logs
	print("Aquiring log list... ", end="", flush=True)
	file_list = os.listdir(path["local"])
	log_list = []
	for item in file_list:
		if log_re.match(item) != None:
			log_list.append(item)
	del file_list
	print(len(log_list))
	
	# Push logs
	for item in log_list:
		print("Pushing log {}... ".format(item), end="", flush=True)
		shutil.move("{}/{}".format(path["local"], item), "//{}/{}/{}".format(address["sage"], path["remote"], item))
		print("Done")
	
	# Clean up
	print("Tidying up... ", end="", flush=True)
	if os.listdir(path["local"]) == []:
		os.rmdir(path["local"])
	print("Done")

if __name__ == "__main__":
	alive = True
	while alive:
		print(">>", end=" ")
		cmd = input()
		if re.match("exit", cmd) != None:
			alive = False
		elif re.match("help", cmd) != None:
			print("help\tDisplay this useage guide")
			print("pull\tPull logs from x-ray machine(s) to local machine")
			print("push\tPush logs from local machine to processing queue")
			print("exit\tQuit the application") 
			print()
		elif re.match("pull", cmd) != None:
			pull_logs()
			print()
		elif re.match("push", cmd) != None:
			push_logs()
			print()
		else:
			if cmd != "":
				print("Command \"{}\" not recognized.".format(cmd))
			print("Type \"help\" for usage information.", end="\n\n")
