# DO NOT EDIT && DO NOT EDIT && DO NOT EDIT && DO NOT EDIT && DO NOT EDIT
# University of Minnesota Solar Vehicle Project
# May 2021

# Base our docker instance on the LTS version of Jenkins
FROM jenkins/jenkins:lts

# Switch to root user
USER root
# Install CMake, Arm toolchain, and clang-format installer requirements
RUN apt update && apt install -y cmake gcc-arm-none-eabi lsb-release wget software-properties-common
# Download llvm's automated apt installer
RUN wget -O llvm.sh https://apt.llvm.org/llvm.sh
# chmod it to be executed
RUN chmod +x llvm.sh
# Set up clang apt repositories
RUN ./llvm.sh 12
# Install clang-format
RUN apt install -y clang-format-12
# Install Jenkins plugins
RUN jenkins-plugin-cli --plugins github-branch-source
# Switch back to jenkins user
USER jenkins

# Set up the jenkins_home volume to persist Jenkins configuration after any
# changes have been made.
# Note: Per Docker docs "If any build steps change the data within the
# volume after it has been declared, those changes will be discarded"
VOLUME [ "/var/jenkins_home" ]

# DO NOT EDIT && DO NOT EDIT && DO NOT EDIT && DO NOT EDIT && DO NOT EDIT
