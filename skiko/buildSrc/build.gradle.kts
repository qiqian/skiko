plugins {
    `kotlin-dsl`
}

repositories {
    maven("https://maven.qq-home.com:8443/repository/maven-central/")
    maven("https://maven.qq-home.com:8443/repository/gradle-plugin-portal/")
}

dependencies {
    implementation(kotlin("stdlib"))
    compileOnly(gradleApi())
    implementation(kotlin("gradle-plugin", "1.9.21"))
    implementation("de.undercouch:gradle-download-task:5.5.0")
}
