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
            <form action="create_manageproject_result.php" method="post">
                <p>Project ID: <input type="text" name="ProjectID" value = "" /></p>
                <p>Project Manager ID: <input type="text" name="ProjectManagerID" value = "" /></p>
                <p><input type="submit" value = "Create Record"/></p>
            </form>

        <?php
        }
        $conn->close();
    ?>

</body>
</html>
