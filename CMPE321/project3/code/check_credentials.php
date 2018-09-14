<?php
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "cmpe321hw3";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }else{
        $username = $_POST["username"];
        $pass = $_POST["password"];

        $username = $conn->real_escape_string($username);
        $pass = $conn->real_escape_string($pass);

        $sql = "SELECT ID, Username, AdminRole FROM user WHERE Username = '" . $username . "' AND Password = '" . $pass . "' ";
        $result = $conn->query($sql);

        if ($result->num_rows > 0) {
            session_start();
            $_SESSION['username'] = $username;
            $row = $result->fetch_assoc();
            $_SESSION['AdminRole'] = $row["AdminRole"];
            $_SESSION['ID'] = $row["ID"];

            header("Location: index.php");
            die();
        } else {
            $conn->close();
            die("Wrong username or password");
        }
    }
?>