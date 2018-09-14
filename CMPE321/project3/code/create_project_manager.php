<html>
<head>
</head>
<body>

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
        ?>
            <form action="create_project_manager_result.php" method="post">
                <p>User name: <input type="text" name="Username" value = "" /></p>
                <p>Password: <input type="text" name="Password" value = "" /></p>
                <p><input type="submit" value = "Create Record"/></p>
            </form>

        <?php
        }
        $conn->close();
    ?>

</body>
</html>
