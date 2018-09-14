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

            // Fetch the record
            $sql = "SELECT ID, Username, Password FROM user WHERE ID = " . $_GET['id'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="delete_project_manager_result.php" method="post">
                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    Are you sure you want to delete the following record? <br />
                    <p>ID: <input type="text" name="ID" value = "<?php echo $row["ID"] ?>" readonly /></p>
                    <p>User name: <input type="text" name="Username" value = "<?php echo $row["Username"] ?>" readonly /></p>
                    <p>Password: <input type="text" name="Password" value = "<?php echo $row["Password"] ?>" readonly /></p>
                    <p><input type="submit" value = "Delete Record" /></p>
                </form>
                <?php
            } else {
                echo "Record does not exist";
            }
        }
        $conn->close();
    ?>

</body>
</html>
