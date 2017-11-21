/*
Navicat MySQL Data Transfer

Source Server         : 172.26.20.99
Source Server Version : 50536
Source Host           : 172.26.20.99:3306
Source Database       : dbtool

Target Server Type    : MYSQL
Target Server Version : 50536
File Encoding         : 65001

Date: 2017-11-21 16:52:11
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `Teacher`
-- ----------------------------
DROP TABLE IF EXISTS `Teacher`;
CREATE TABLE `Teacher` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `age` int(11) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `salary` float(11,0) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of Teacher
-- ----------------------------
INSERT INTO `Teacher` VALUES ('1', '15', 'THWqs', '50');
INSERT INTO `Teacher` VALUES ('2', '30', 'THW', '50');
INSERT INTO `Teacher` VALUES ('3', '30', 'THW', '50');
INSERT INTO `Teacher` VALUES ('4', '30', 'THW', '50');
