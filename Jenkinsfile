node {

  try {
    currentBuild.result = "SUCCESS"
    env.AWS_ECR_LOGIN = true
  
    checkout scm

    docker.image('ubuntu:16.04').inside('-u root') {

      
      
      stage('Setup') {
        configFileProvider([configFile(fileId: 'test-env-file', targetLocation: './')]) {
          load './test.properties'
        }
        sh 'apt-get update && apt-get install -y build-essential'
        sh 'apt-get install -y cmake'
      }
      
      stage('Build') {
        sh 'cmake .'
        sh 'make'
      }
      
      stage('Test') {
        
      }

      stage('Analysis') {
        
      }
      
    }

    stage('Deploy to Staging') {
      if (env.BRANCH_NAME == 'staging') {
        echo 'Building and pushing image'
        docker.withRegistry('https://453230908534.dkr.ecr.ap-south-1.amazonaws.com/tracified/grammar-staging', 'ecr:ap-south-1:aws-ecr-credentials') {
          echo 'Building image'
          def releaseImage = docker.build("tracified/grammar-staging:${env.BUILD_ID}")
          releaseImage.push()
          releaseImage.push('latest')
        }
        echo 'Deploying image in server'
        withCredentials([[
          $class: 'AmazonWebServicesCredentialsBinding', 
          accessKeyVariable: 'AWS_ACCESS_KEY_ID', 
          credentialsId: 'aws-ecr-credentials', 
          secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
        ]]) {
          ansiblePlaybook inventory: 'deploy/hosts', playbook: 'deploy/staging.yml', extras: '-u ubuntu'
        }
      }
    }

    stage('Deploy to Production') {
      if (env.BRANCH_NAME == 'release') {
        echo 'Building and pushing image'
        docker.withRegistry('https://453230908534.dkr.ecr.ap-south-1.amazonaws.com/tracified/grammar', 'ecr:ap-south-1:aws-ecr-credentials') {
          echo 'Building image'
          def releaseImage = docker.build("tracified/grammar:${env.BUILD_ID}")
          releaseImage.push()
          releaseImage.push('latest')
        }
        echo 'Deploying image in server'
        withCredentials([[
          $class: 'AmazonWebServicesCredentialsBinding', 
          accessKeyVariable: 'AWS_ACCESS_KEY_ID', 
          credentialsId: 'aws-ecr-credentials', 
          secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
        ]]) {
          ansiblePlaybook inventory: 'deploy/hosts', playbook: 'deploy/production.yml', extras: '-u ubuntu'
        }
      }
    }
  }
  catch (exc) {
    currentBuild.result = "FAILURE"
    echo 'Something went wrong'
  }
  finally {
    echo 'All done. Cleaning up docker'
    sh 'docker system prune -af'
    echo "Completed pipeline: ${currentBuild.fullDisplayName} with status of ${currentBuild.result}"
  }

}