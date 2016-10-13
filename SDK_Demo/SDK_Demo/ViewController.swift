//
//  ViewController.swift
//  SDK_Demo
//
//  Created by pxh on 2016/10/12.
//  Copyright © 2016年 pxh. All rights reserved.
//

import UIKit
let cellIndentifier:String = "MainTableViewCell"
class ViewController: UIViewController,UITableViewDelegate,UITableViewDataSource{
    var itmes : NSMutableArray?
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var tipLabel: UILabel!
    @IBOutlet weak var tipBg: UIView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        self.resetData()
        self.setNavigation()
        self.addMainTableView()
    }
    func resetData() {
        self.itmes = NSMutableArray.init(array: ["语音理解","声纹识别"])
    }
    func setNavigation(){
        self.title = "云知声只能语音交互平台"
        self.tipBg.layer.borderWidth = 1
        self.tipBg.layer.cornerRadius = 5
        self.tipBg.layer.borderColor = UIColor.black.cgColor
    }
    func addMainTableView(){
        self.tableView!.delegate = self;
        self.tableView!.dataSource = self;
        self.tableView.register(UITableViewCell.classForCoder(), forCellReuseIdentifier: cellIndentifier)
        self.view.addSubview(self.tableView!)
    }
    //Mark: UITableviewDelegate

    func tableView(_ tableView: UITableView, didEndDisplayingHeaderView view: UIView, forSection section: Int){
    
    }
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath){
    
    }
    //Mark : UITableviewDataSourse
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return (self.itmes?.count)!
    }
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell:UITableViewCell = tableView.dequeueReusableCell(withIdentifier: cellIndentifier, for: indexPath)
        let title = self.itmes?[indexPath.row]
        cell.textLabel?.text = title as! String?
        return cell
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

