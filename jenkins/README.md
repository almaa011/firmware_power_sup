# Docker Setup #

## ece-solar.ece.umn.edu ##
We have a virtual private server from the university with Orser's permission. Once you have access to the server you should be able to switch to the root user and view a checked out copy of the firmware repo.

* `sudo su root`
* `cd ~/firmware/jenkins`
* `git checkout master-f1`
  - Use current stable branch (master-f1 as of spring 2022).
* `git pull`
  - Will require your username and password for UMN github.

## Building the Docker Image ##
In order to easily set up the Jenkins server, we have a `Dockerfile` that helps prepare a Jenkins docker image for our setup.

To build our image:
* `cd firmware/jenkins/`
* `docker pull jenkins/jenkins:lts`
* `docker build -t svp/jenkins .`

This will create a local image that is a copy of the _jenkins/jenkins:lts_ image, but with our special tweaks applied.
This includes `apt` packages, Jenkins plugins, and other Jenkins configuration.
Additionally, this will install `clang-format-12` for our formatting pipeline.
More documentation can be found in the `Dockerfile`.

## Starting an Instance ##
Once an image is built, we want to actually run our image.
* `docker run --detach --restart always --publish 8080:8080 --volume jenkins_home:/var/jenkins_home svp/jenkins:latest`
  - `--detach` Specifies that the instance should detach from the terminal and run in the background.
  - `--restart` Specifies that the docker instance should always restart on any crash or power cycle and will only pause if manually stopped.
  - `--publish` Specifies that port 8080 of the host should go to port 8080 of the docker instance. This port will have to be allowed through any firewalls to the host.
  - `--volume` Specifies that the files in /var/jenkins_home on the docker image will persist past the lifetime of the docker image. This means we can quickly transfer configuration as the docker is updated.

Once you have an instance running, we can configure it using the instructions in the _Jenkins Setup_ section.

# Jenkins Setup #
## Useful Links ##
* https://www.djaodjin.com/blog/jenkins-build-pull-requests.blog.html
* https://devopscube.com/jenkins-build-trigger-github-pull-request/
* https://www.jenkins.io/doc/pipeline/tour/running-multiple-steps/
* https://www.jenkins.io/doc/tutorials/build-a-multibranch-pipeline-project/
* https://www.jenkins.io/doc/pipeline/tour/tests-and-artifacts/

## Initial Setup ##
Navigate to the url of the Jenkins instance.
Our current instance is located at: http://ece-solar.ece.umn.edu:8080/

In order to get the initial admin password run:

`docker exec <instance_name> cat /var/jenkins_home/secrets/initialAdminPassword`

Install suggested plugins, some plugins may fail, make sure these are not plugins related to git, ssh, or pipelines otherwise you may have problems.

## Jenkins Github Webhook ##
This will integrate Github and Jenkins, allowing Github to notify Jenkins of changes to the repository.

Go to _Settings_ -> _Hooks_ -> _Add webhook_

* _Payload URL_
  - Our current Docker image uses:
  - `http://ece-solar.ece.umn.edu:8080/github-webhook/`
* _Content Type_
  - `application/json`
* _Secret_
  - Generate a long password
  - On Linux this can be done with:
  - `head /dev/urandom | tr -dc 'A-Za-z0-9!@#$%^&*' | head -c 64; echo`
  - Jenkins will also need this secret later
* _Which events would you like to trigger this webhook?_
  - `Select individual events`
  - Enable these events:
    - `Branch or tag creation`
    - `Branch or tag deletion`
    - `Pull requests`
    - `Pull request reviews`
    - `Pull request review comments`
    - `Statuses`
    - `Pushes`

Save the webhook and check for a green checkmark in the _Recent Deliveries_ section. This will tell you if Github was able to talk to the Jenkins instance.

## Jenkins Github Authentication ##
Jenkins also needs a way to access the Github repository.
A separate account dedicated to Jenkins should be used which involves talking to UMN IT.
Since Jenkins needs access to the Github API, one will need to obtain a _Personal Access Token_ for the Jenkins specific account.
The personal access token will allow Jenkins to access the repository without using a personal password.

This section will also set the secret for the Github webhook in Jenkins.

From the Jenkins Dashboard, go to _Manage Jenkins_ -> _Security_ -> _Manage Credentials_.
Under _Stores scoped to Jenkins_, go to the _Jenkins_ row and click on the _(global)_ domain.

Click on _Add Credentials_. We will now add two credentials.
* Personal Access Token
  - _Kind_
    - `Username with password`
  - _Scope_
    - `Global`
  - _Username_
    - The username used for the Personal Access Token
  - _Password_
    - The token given by Github
  - _ID_
    - Leave blank or as default
  - _Description_
    - Up to you but descriptions help identify the credential
    - `Jenkins Personal Access Token`
* Github Webhook Secret
  - _Kind_
    - `Secret Text`
  - _Scope_
    - `Global`
  - _Secret_
    - The secret you generated for the Github Webhook
  - _ID_
    - Leave blank or as default
  - _Description_
    - `Jenkins Webhook Secret`

### Setting the Jenkins Webhook Secret ###
After adding the Jenkins Webhook Secret to the credentials provider, you will also have to tell Jenkins to use that secret.
From the Jenkins dashboard, go to _Manage Jenkins_ -> _System Configuration_ -> _Configure System_
Find the _GitHub_ section, click on _Advanced_. From here you should be able to add a shared secret and use the previously configured secret.

## Installed Plugins ##

These plugins are automatically installed in the Dockerfile but are listed here for reference

* https://plugins.jenkins.io/github-branch-source/

## Jenkins Configuration ##
* Create a new _Multibranch Pipeline_
  - Note: This is not a _Multibranch Pipeline with defaults_
* **Branch Sources**
  - **GitHub**
    - **Credentials**
      - Use the personal access token configure previously
    - **Behaviors**
    - _Discover branches_
      - `Only branches that are also filed as PRs`
    - **Property strategy**
      - `All branches get the same properties`
  - **GitHub**
    - **Credentials**
      - Use the personal access token configure previously
    - **Behaviors**
    - _Discover branches_
      - `All branches`
    - _Filter by name (with wildcards)_
      - _Include_
        * `master-* development-*`
      - _Exclude_
        - Leave Blank
    - **Property strategy**
      - `All branches get the same properties`
* **Build Configuration**
  - **Mode**
  - `by Jenkinsfile`
  - _Script Path_
    - `jenkins/Jenkinsfile`
    - The `Jenkinsfile` is a file that describes how to build the software in the Jenkins pipeline.
* **Orphaned Item Strategy**
  - Discard old items
    - Days to keep old items
      - 60
  - Max # of old items to keep
    - Leave Empty

# Upgrading Jenkins #

Since our Jenkins docker image was started using a volume to store the Jenkins data, we should be able to start a new docker image with an upgraded Jenkins using the configuration from the volume

* Update the docker image
  - Follow the same instructions as [above](#Building-the-Docker-Image) to rebuild a new version of our docker image

* Stop the old docker instance
  - `docker ps` to find the Jenkins docker instance.
  - `docker stop <instance_name>` stops the specified instance

* Back up the old docker data
  - Use `docker inspect <instance_name>` to find where the `jenkins_home` volume is stored
  - This is typically under `/var/lib/docker/volumes/jenkins_home/_data` on the host
    but can be seen under the `"Mounts"` key in the output.
  - Make a copy of directory and save it somewhere safe.
    - Making a zip is quick way to get an archive of the data.
    - `zip -r jenkins-<date>.zip /var/lib/docker/volumes/jenkins_home/_data`
  - If the update goes wrong, this copy will help restore the configurations quickly

* Start a new instance using the existing volume
  - See the [`docker run`](##Starting-an-Instance) command above to start the new instance.

* Update all the plugins from the Jenkins GUI
  - Go to _Manage Jenkins_ -> _Manage Plugins_
  - Select all plugins in the _Updates_ tab and update them
  - Restart docker instance
  - (Optional) Restart host machine
